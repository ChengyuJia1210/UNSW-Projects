// reln.c ... functions on Relations
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "defs.h"
#include "reln.h"
#include "page.h"
#include "tuple.h"
#include "tsig.h"
#include "bits.h"
#include "hash.h"
#include "psig.h"

/*
 *   2018/10/20 version
 *
 *
*/
PageID addBSigPage(Reln);
// open a file with a specified suffix
// - always open for both reading and writing

File openFile(char *name, char *suffix)
{
	char fname[MAXFILENAME];
	sprintf(fname,"%s.%s",name,suffix);
	File f = open(fname,O_RDWR|O_CREAT,0644);
	assert(f >= 0);
	return f;
}

// create a new relation (five files)
// data file has one empty data page

Status newRelation(char *name, Count nattrs, float pF,
                   Count tk, Count tm, Count pm, Count bm)
{
	Reln r = malloc(sizeof(RelnRep));
	RelnParams *p = &(r->params);
	assert(r != NULL);
	p->nattrs = nattrs;
	p->pF = pF,
	p->tupsize = 28 + 7*(nattrs-2);
	p->tupPP = (PAGESIZE-sizeof(Count))/p->tupsize;
	p->tk = tk; 
	if (tm%8 > 0) tm += 8-(tm%8); // round up to byte size
	p->tm = tm; p->tsigSize = tm/8; p->tsigPP = PAGESIZE/(tm/8);
	if (pm%8 > 0) pm += 8-(pm%8); // round up to byte size
	p->pm = pm; p->psigSize = pm/8; p->psigPP = PAGESIZE/(pm/8);
	if (p->psigPP < 2) { free(r); return -1; }
	if (bm%8 > 0) bm += 8-(bm%8); // round up to byte size
	p->bm = bm; p->bsigSize = bm/8; p->bsigPP = PAGESIZE/(bm/8);
	if (p->bsigPP < 2) { free(r); return -1; }
	r->infof = openFile(name,"info");
	r->dataf = openFile(name,"data");
	r->tsigf = openFile(name,"tsig");
	r->psigf = openFile(name,"psig");
	r->bsigf = openFile(name,"bsig");
	addPage(r->dataf); p->npages = 1; p->ntups = 0;
	addPage(r->tsigf); p->tsigNpages = 1; p->ntsigs = 0;
	addPage(r->psigf); p->psigNpages = 1; p->npsigs = 0;
	addPage(r->bsigf); p->bsigNpages = 1; p->nbsigs = 0; // replace this
	// Create a file containing "pm" all-zeroes bit-strings,
    // each of which has length "bm" bits
	p->nbsigs = p->pm;
	addBSigPage(r);

	closeRelation(r);
	return 0;
} 


// check whether a relation already exists

Bool existsRelation(char *name)
{
	char fname[MAXFILENAME];
	sprintf(fname,"%s.info",name);
	File f = open(fname,O_RDONLY);
	if (f < 0)
		return FALSE;
	else {
		close(f);
		return TRUE;
	}
}

// set up a relation descriptor from relation name
// open files, reads information from rel.info

Reln openRelation(char *name)
{
	Reln r = malloc(sizeof(RelnRep));
	assert(r != NULL);
	r->infof = openFile(name,"info");
	r->dataf = openFile(name,"data");
	r->tsigf = openFile(name,"tsig");
	r->psigf = openFile(name,"psig");
	r->bsigf = openFile(name,"bsig");
	read(r->infof, &(r->params), sizeof(RelnParams));
	return r;
}

// release files and descriptor for an open relation
// copy latest information to .info file
// note: we don't write ChoiceVector since it doesn't change

void closeRelation(Reln r)
{
	// make sure updated global data is put in info file
	lseek(r->infof, 0, SEEK_SET);
	int n = write(r->infof, &(r->params), sizeof(RelnParams));
	assert(n == sizeof(RelnParams));
	close(r->infof); close(r->dataf);
	close(r->tsigf); close(r->psigf); close(r->bsigf);
	free(r);
}

//------------------------------------tuple signature relation-------------------------------------------------------
// make tuple signature and put those signatures into the signature file
Status addTupleSignatureToPage(Reln r, Page p, Bits t)
{
	if (pageNitems(p) == maxTsigsPP(r)) return NOT_OK;
	int sigSize = r->params.tsigSize;// each tuple signature's size
	Byte *addr = addrInPage(p, pageNitems(p), sigSize);
	memcpy(addr, &(t->bitstring[0]), sigSize);
	addOneItem(p);
	return OK;
}

PageID addTupleSig(Reln r, Tuple t){
	Bits tSig = makeTupleSig(r, t);
	Page p;  PageID pid;
	RelnParams *rp = &(r->params);
	pid = rp->tsigNpages-1;
	p = getPage(r->tsigf, pid);
	// check if room on last page; if not add new page
	if (pageNitems(p) == rp->tsigPP) {
		addPage(r->tsigf);
		rp->tsigNpages++;
		pid++;
		free(p);
		p = newPage();
		if (p == NULL) return NO_PAGE;
	}
	addTupleSignatureToPage(r, p, tSig);
	rp->ntsigs++;  //written to disk in closeRelation()
	putPage(r->tsigf, pid, p);
	return pid;
}
//--------------------------------------------------------------------------------------------------------


//-----------------------------------page signature rlation-----------------------------------------------
Bits getPsignatureFromPage(Reln r, Page p, int i)
{
	assert(r != NULL && p != NULL);
	assert(i <= pageNitems(p));
	int sigSize = r->params.psigSize;
	Byte *addr = addrInPage(p, i, sigSize);
	Bits signature = newBits(sigSize*8);
	memcpy(&(signature->bitstring[0]), addr, sigSize);
	return signature;
}

Status addPageSignatureToPage(Reln r, Page p, Bits t, int ithItem, int flag)
{
	if (ithItem == maxPsigsPP(r)) return NOT_OK;
	int sigSize = r->params.psigSize;// each page signature's size
	Byte *addr = addrInPage(p, ithItem, sigSize);
	memcpy(addr, &(t->bitstring[0]), sigSize);
	if(flag == 1){
		addOneItem(p);
	}
	return OK;
}

//PageID addPsigToPage(Reln r, Tuple t, PageID pid){// introduce the data page id
Bits addPsigToPage(Reln r, Tuple t, PageID pid){// introduce the data page id
	//int i, j;
	// produce this tuple's page signature
	Bits pSig = makePageSig(r, t);
	Bits pageSig;// the page signature which is fetched from the page signature file

	Page psigPage; //get the current page signature's page
	PageID currentPsigpageID; 

	


	RelnParams *rp = &(r->params);
	currentPsigpageID = rp->psigNpages-1;//calculate the current page signature pageID
	psigPage = getPage(r->psigf, currentPsigpageID);
	// get current page signature's index	
	int currentPsigNum;

	/* because the data page's number always equal to the page signatures' number
	 * if pid == npsigs means we still processing this perticular page's tuples */
	if(pid == rp->npsigs){// && rp->ntups != 1){
		currentPsigNum = (pid-1) - currentPsigpageID*(rp->psigPP);
		pageSig = getPsignatureFromPage(r, psigPage, currentPsigNum);
		orBits(pageSig, pSig);

		addPageSignatureToPage(r, psigPage, pageSig, currentPsigNum, 0);
		putPage(r->psigf, currentPsigpageID, psigPage);
		free(pSig);
		return pageSig;
	}
	/*else, we processing next page's tuples, so npsigs++*/
	else{
		if (pageNitems(psigPage) == rp->psigPP) {
			addPage(r->psigf);
			rp->psigNpages++;
			currentPsigpageID++;
			free(psigPage);
			psigPage = newPage();
			if (psigPage == NULL) return NULL;// NO_PAGE;
		}
		currentPsigNum = (pid-1) - currentPsigpageID*(rp->psigPP);
		addPageSignatureToPage(r, psigPage, pSig, currentPsigNum, 1);		
		rp->npsigs++;
		putPage(r->psigf, currentPsigpageID, psigPage);
		return pSig;
	}
	//return currentPsigpageID;
	 
}



//--------------------------------------------------------------------------------------------------------
//-----------------------------------bit-slices signature rlation-----------------------------------------
PageID addBSigPage(Reln r){
	RelnParams *rp = &(r->params);
	Count pm = rp->pm;
	unsigned int i;
	Page bSigPage;
	PageID pid = rp->bsigNpages-1;;
	/* this is the bit-slices signature, 
	 * the width is equal to "bm", 
	 * which is the data page numbers*/
	Bits emptyBsig = newBits(rp->bm);
	bSigPage = getPage(bsigFile(r), pid);
	int count = 0;

	for(i = 0; i < pm; i++){
		/* if number of bit-slices signature bigger than max bsig per page
		 * add one more page, else insert the empty bSig in the same page*/
		if (pageNitems(bSigPage) == rp->bsigPP) {
			putPage(bsigFile(r), pid, bSigPage);//put page into the disk and apply new page
			addPage(r->bsigf);
			rp->bsigNpages++;
			pid++;
			bSigPage = newPage();
			count++;
			if (bSigPage == NULL) return NO_PAGE;
		}
		putBits(bSigPage, i%rp->bsigPP, emptyBsig);
		//printf("nitems: %d\n", pageNitems(bSigPage));
		addOneItem(bSigPage);
	}
	if(count < rp->bsigNpages){
		putPage(bsigFile(r), pid, bSigPage);
		count++;
	}
	return pid;
}

void addBsigToPage(Reln r, Bits pSig, Count CurrentPage){
	Count pm = psigBits(r);
	Count bm = bsigBits(r);
	Page bSigPage;
	unsigned int i;

	Bits getDataBuf = newBits(bm);
	PageID pid;
	Offset position;

	for(i = 0; i < pm; i++){//scan through the whole page signature's data
		if(bitreader(i, pSig) > 0){
			pid = i/maxBsigsPP(r);
			position = i%maxBsigsPP(r);

			bSigPage = getPage(bsigFile(r), pid);

			getBits(bSigPage, position, getDataBuf);
			bitwriter((CurrentPage-1), getDataBuf, 1);
			putBits(bSigPage, position, getDataBuf);
			putPage(bsigFile(r), pid, bSigPage);
		}
	}
	free(getDataBuf);
}
//--------------------------------------------------------------------------------------------------------


// insert a new tuple into a relation
// returns page where inserted
// returns NO_PAGE if insert fails completely

PageID addToRelation(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL && strlen(t) == tupSize(r));
	Page p;  PageID pid;
	RelnParams *rp = &(r->params);
	
	// add tuple to last page
	pid = rp->npages-1;
	p = getPage(r->dataf, pid);
	// check if room on last page; if not add new page
	if (pageNitems(p) == rp->tupPP) {
		addPage(r->dataf);
		rp->npages++;
		pid++;
		free(p);
		p = newPage();
		if (p == NULL) return NO_PAGE;
	}
	addTupleToPage(r, p, t);
	rp->ntups++;  //written to disk in closeRelation()
	putPage(r->dataf, pid, p);

	// compute tuple signature and add to tsigf
	addTupleSig(r, t);

	// compute page signature and add to psigf
	/* Here pid add one means the current page number 
	 * for the data page, that we want to construct 
	 * the page signature*/  
	Bits pSig = addPsigToPage(r, t, pid+1);

	// use page signature to update bit-slices
	addBsigToPage(r, pSig, pid+1);
	free(pSig);

	return nPages(r)-1;
}

// displays info about open Reln (for debugging)

void relationStats(Reln r)
{
	RelnParams *p = &(r->params);
	printf("Global Info:\n");
	printf("Dynamic:\n");
    printf("  #items:  tuples: %d  tsigs: %d  psigs: %d  bsigs: %d\n",
			p->ntups, p->ntsigs, p->npsigs, p->nbsigs);
    printf("  #pages:  tuples: %d  tsigs: %d  psigs: %d  bsigs: %d\n",
			p->npages, p->tsigNpages, p->psigNpages, p->bsigNpages);
	printf("Static:\n");
    printf("  tups   #attrs: %d  size: %d bytes  max/page: %d\n",
			p->nattrs, p->tupsize, p->tupPP);
	printf("  sigs   bits/attr: %d\n", p->tk);
	printf("  tsigs  size: %d bits (%d bytes)  max/page: %d\n",
			p->tm, p->tsigSize, p->tsigPP);
	printf("  psigs  size: %d bits (%d bytes)  max/page: %d\n",
			p->pm, p->psigSize, p->psigPP);
	printf("  bsigs  size: %d bits (%d bytes)  max/page: %d\n",
			p->bm, p->bsigSize, p->bsigPP);
}
