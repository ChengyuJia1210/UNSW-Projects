// bits.c ... functions on bit-strings
// part of SIMC signature files
// Bit-strings are arbitrarily long byte arrays
// Least significant bits (LSB) are in array[0]
// Most significant bits (MSB) are in array[nbytes-1]

// Written by John Shepherd, September 2018

#include <assert.h>
#include "defs.h"
#include "bits.h"
#include "page.h"

int bitreader(int position, Bits b){
  int flag = 0;
  int i = position/8;
  int j = position%8;
  
  flag = ((1 << j) & b->bitstring[i]);
  if(flag != 0){
    return 1;
  }
  else{
    return 0;
  }
}

void bitwriter(int position, Bits b, int flag){
  int i = position/8;//which block
  int j = position%8;//which position in this block
  if(flag == 1){
    b->bitstring[i] = ((1 << j) | b->bitstring[i]);
  }
  else{
    b->bitstring[i] = ((~(1 << j)) & b->bitstring[i]);
  }
}

void bitAnder(Bits a, Bits b){
	int len = a->nbits;
	int i;
	int aid, bid;
	for(i = 0; i < len; i++){
		aid = bitreader(i, a);
		bid = bitreader(i, b);
		if(aid == 1 && bid == 0){
			bitwriter(i, a, 0);
		}
	}
}

void bitOrer(Bits a, Bits b){
	int len = a->nbits;
	int i;
	int bid;

	for(i = 0; i < len; i++){
		bid = bitreader(i, b);
		if(bid == 1){
			bitwriter(i, a, 1);
		}
	}
}

// create a new Bits object

Bits newBits(int nbits)
{
	Count nbytes = iceil(nbits,8);
	Bits new = malloc(2*sizeof(Count) + nbytes);
	new->nbits = nbits;
	new->nbytes = nbytes;
	memset(&(new->bitstring[0]), 0, nbytes);
	return new;
}

// release memory associated with a Bits object

void freeBits(Bits b)
{
	free(b);
}

// check if the bit at position is 1

Bool bitIsSet(Bits b, int position)
{
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	if(bitreader(position, b)){
	  return TRUE;
	}
	return FALSE; // remove this
}

// check whether one Bits b1 is a subset of Bits b2

Bool isSubset(Bits b1, Bits b2)
{
	assert(b1 != NULL && b2 != NULL);
	assert(b1->nbytes == b2->nbytes);
	int i;
	int b1ID, b2ID;
	for(i = 0; i < b1->nbits; i++){
		b1ID = bitreader(i, b1);
		b2ID = bitreader(i, b2);
		if(b1ID == 0 && b2ID == 1){//(b1ID == 1 && b2ID == 0)
			return FALSE;
		}
	}
	
	return TRUE; // remove this
}

// set the bit at position to 1

void setBit(Bits b, int position)//done
{
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	bitwriter(position, b, 1);
}

// set all bits to 1

void setAllBits(Bits b)//Done
{
	assert(b != NULL);
	for (int i = 0; i < b->nbits; i++) {
	  bitwriter(i, b, 1);  
	}
}

// set the bit at position to 0

void unsetBit(Bits b, int position)//done
{
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	bitwriter(position, b, 0);
}

// set all bits to 0

void unsetAllBits(Bits b)//done
{
	assert(b != NULL);
	for (int i = 0; i < b->nbits; i++) 
	    bitwriter(i, b, 0);
}

// bitwise AND ... b1 = b1 & b2

void andBits(Bits b1, Bits b2) //done
{
	assert(b1 != NULL && b2 != NULL);
	assert(b1->nbytes == b2->nbytes);
	bitAnder(b1, b2);
}

// bitwise OR ... b1 = b1 | b2

void orBits(Bits b1, Bits b2) //done
{
	assert(b1 != NULL && b2 != NULL);
	assert(b1->nbytes == b2->nbytes);
	bitOrer(b1, b2);
}


// get a bit-string (of length b->nbytes)
// from specified position in Page buffer
// and place it in a BitsRep structure

void getBits(Page p, Offset pos, Bits b)
{
	Count size = b->nbytes;
	Byte* addr = addrInPage(p, (int)pos, (int)size);
	memcpy(&(b->bitstring[0]), addr, size);
}

// copy the bit-string array in a BitsRep
// structure to specified position in Page buffer

void putBits(Page p, Offset pos, Bits b)
{
	Count size = b->nbytes;
	Byte* addr = addrInPage(p, (int)pos, (int)size);
	memcpy(addr, &(b->bitstring[0]), size);
}


// show Bits on stdout
// display in order MSB to LSB
// do not append '\n'

void showBits(Bits b)
{
	assert(b != NULL);
	for (int i = b->nbytes-1; i >= 0; i--) {
		for (int j = 7; j >= 0; j--) {
			Byte mask = (1 << j);
			if (b->bitstring[i] & mask)
				putchar('1');
			else
				putchar('0');
		}
	}
}
