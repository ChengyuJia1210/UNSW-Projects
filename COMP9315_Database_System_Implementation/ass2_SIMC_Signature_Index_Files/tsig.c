// tsig.c ... functions on Tuple Signatures (tsig's)
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include <unistd.h>
#include <string.h>
#include "defs.h"
#include "tsig.h"
#include "reln.h"
#include "hash.h"
#include "bits.h"


#define matches(rdesc,qdesc) (isSubset(rdesc, qdesc) == TRUE)

Bits codeWord(char* attr, Count m, Count k){//, Reln r
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


Bits getSignatureFromPage(Reln r, Page p, int i)
{
	assert(r != NULL && p != NULL);
	assert(i <= pageNitems(p));
	int sigSize = r->params.tsigSize;
	Byte *addr = addrInPage(p, i, sigSize);
	Bits signature = newBits(sigSize*8);
	memcpy(&(signature->bitstring[0]), addr, sigSize);
	return signature;
}



// make a tuple signature

Bits makeTupleSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);
	Count numAtr = nAttrs(r);
	Count k = codeBits(r);
	Count m = tsigBits(r);
	Bits tSig = newBits(m);//initial tuple sigature describor;
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
			eachSig = codeWord(attribute, m, k);
			orBits(tSig, eachSig);
		}
	}
	
	return tSig;
}



// find "matching" pages using tuple signatures

void findPagesUsingTupSigs(Query q)
{
	assert(q != NULL);
	Reln r = q->rel;
	Bits querySig = makeTupleSig(r, (Tuple)q->qstring);
	Bits tupleSig;
	// used as bit map to store the position of the page id that need to be checked in the page scaning.
	//Bits examPageId = newBits(nTuples(r));
	Bits examPageId = newBits(nPages(r));
	unsigned int count = 0;	

	int i;//, flag;
	Page p; 
	//Tuple t;
	Bool Match;
	for(q->curpage = 0; q->curpage < nTsigPages(r); q->curpage++){
		p = getPage(tsigFile(r), q->curpage);//(r->tsigf, q->curpage);
		for (i = 0; i < pageNitems(p); i++) {
			tupleSig = getSignatureFromPage(r, p, i);
			Match = matches(tupleSig, querySig);
			q->nsigs++;
			if(Match == TRUE){
				bitwriter(count/maxTupsPP(r), examPageId, 1);
			}
			free(tupleSig);
			//count number means tuple number
			count++;
		}
		q->nsigpages++;
		free(p);
	}	
	free(querySig);

	q->pages = examPageId;
	printf("Matched Pages:"); showBits(q->pages); putchar('\n');
}
