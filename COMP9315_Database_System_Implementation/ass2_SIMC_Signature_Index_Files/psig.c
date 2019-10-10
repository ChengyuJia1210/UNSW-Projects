// psig.c ... functions on page signatures (psig's)
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "psig.h"

#include "hash.h"
#define matches(rdesc,qdesc) (isSubset(rdesc, qdesc) == TRUE)

/************************************************************************
 * This file works for the page signature making(makePageSig function) 
 * and according to the query to make query signature. Then scan through 
 * use the whole page signature file we created before to compare the 
 * query signature. Storing the match page signature id into the exam buffer,
 * which will be checked in the futrue.
 *************************************************************************/

Bits PagecodeWord(char* attr, Count m, Count k){//, Reln r
	Count nbits = 0;
	Bits cWord = newBits(m);
	int i;
		
	srandom(hash_any(attr, strlen(attr)));// set random seed
	while(nbits < k){
		i = random()%m;
		if(bitreader(i, cWord) == 0){
			bitwriter(i, cWord, 1);
			nbits++;
		}
	}
	return cWord;
}
Bits getSignatureFromPage2(Reln r, Page p, int i)
{
	assert(r != NULL && p != NULL);
	assert(i <= pageNitems(p));
	int sigSize = r->params.psigSize;
	Byte *addr = addrInPage(p, i, sigSize);
	Bits signature = newBits(sigSize*8);
	memcpy(&(signature->bitstring[0]), addr, sigSize);
	return signature;
}

// this version makePagesig is according to tuples*tupleAttributes to make page signature 
Bits makePageSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);
	Count k = codeBits(r);
	Count m = psigBits(r);	
	Bits pSig = newBits(m);//initial page sigature describor;
	Count numAtr = nAttrs(r);
	Bits eachSig;//each attribute's signature

	int i;
	char* attribute;
	char** allAttributes;
	

	allAttributes = tupleVals(r, t);
	for(i = 0; i < numAtr; i++){
		attribute = allAttributes[i];
		if(strcmp(attribute, "?") == 0){
			continue;
		}
		else{
			eachSig = PagecodeWord(attribute, m, k);//, r
			orBits(pSig, eachSig);
		}
	}
	
	return pSig;

}

void findPagesUsingPageSigs(Query q)
{
	assert(q != NULL);
	Reln r = q->rel;
	Bits querySig = makePageSig(r, (Tuple)q->qstring);
	Bits pageSig;
	Bits examPageId = newBits(nPsigs(r));

	int i;// i means current signature
	unsigned int count = 0;// count means the pagesigature's number,also equal to the page's number
	Page p; 

	for(q->curpage = 0; q->curpage < nPsigPages(r); q->curpage++){
		p = getPage(psigFile(r), q->curpage);
		for(i = 0; i < pageNitems(p); i++){
			pageSig = getSignatureFromPage2(r, p, i);
			q->nsigs++;
			if(matches(pageSig, querySig) == TRUE){
				bitwriter(count, examPageId, 1);
			}
			count++;
			free(pageSig);
		}
		q->nsigpages++;
		free(p);
	}
	free(querySig);
	q->pages = examPageId;
	printf("Matched Pages: "); showBits(q->pages); putchar('\n');
}

