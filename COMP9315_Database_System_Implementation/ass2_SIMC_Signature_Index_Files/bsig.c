// bsig.c ... functions on Tuple Signatures (bsig's)
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "bsig.h"
#include "psig.h"

void findPagesUsingBitSlices(Query q)
{
	assert(q != NULL);
	//TODO
	Reln r = q->rel;
	Count bm = bsigBits(r);
	Bits checkBits = newBits(bm);// this is the bit map that check which bsig page had been fetched
	Bits bSig = newBits(bm);//this is each row's bit-slices signature	
	Bits bSigReadMap = newBits(nBsigPages(r));
	setAllBits(checkBits);
	int currentRow;
	Page p;
	PageID pid;// bsig page id
	Offset position;

	Bits querySig = makePageSig(r, (Tuple)q->qstring);
	for(currentRow = 0; currentRow < querySig->nbits; currentRow++){
		if(bitIsSet(querySig, currentRow) == TRUE){// if true fetch this row's page, and do the and
			pid = currentRow/maxBsigsPP(r);
			position = currentRow%maxBsigsPP(r);	
	
			p = getPage(bsigFile(r), pid);
			getBits(p, position, bSig);
			
			andBits(checkBits, bSig);// checkBits = (checkBits & bSig)
			q->nsigs++;
			free(p);
			setBit(bSigReadMap, pid);
		}
	}
	q->pages = checkBits;
	free(bSig);

	for(currentRow = 0; currentRow < nBsigPages(r); currentRow++){
		if(bitIsSet(bSigReadMap, currentRow)){
			q->nsigpages++;		
		}
	}
	free(bSigReadMap);
}

