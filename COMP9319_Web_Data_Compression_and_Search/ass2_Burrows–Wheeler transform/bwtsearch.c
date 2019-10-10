#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CAPACITY_SIZE 350 //for small file
#define CAPACITY_SIZE_FOR_LARGE_FILE 3500 //2250// 32bits machine use 2250; 64bits machine use 3500
#define FILE_SIZE_THRESHOLD 5000000//5MB


char Deliminater;

int iceil(int val, int base)
{
	int ceil = val / base;
	if (val%base > 0) ceil++;
	return ceil;
}

void bitWriter(char* bitarr, int k){   //right to left
	int i = k/8;
	int j = k%8;
	bitarr[i] = ((1 << j) | bitarr[i]);
}

int bitreader(char* num, int pos){
	int new = 0;
	int i = pos/8;
	int j = pos%8;
	new = ((1 << j) & num[i]) ;
	if(new != 0){
		return 1;
	}
	else{
		return 0;
	}
}


int** fileoccTable(FILE* bwtf, int fsize, char deliminater, int capacity, int* cTable, int* ct){//include occ table and c table
	int nblocks = 0;
	int i, l, n = 0, j = 0; // "n" is current block, "j" is current capacity
	char alpha;
	
	nblocks = fsize/capacity;
	if(nblocks*capacity < fsize){
		nblocks++;
	}

	int** occ = (int**) calloc(127, sizeof(int*));
	for(i = 0; i < 127; i++){
		occ[i] = (int*) calloc(nblocks, sizeof(int));
	}

	//this part is used to construct the occ table and c table
	for(i = 0; i < fsize; i++){
		alpha = fgetc(bwtf);
		if(alpha == deliminater){
			alpha = 0;
		}
		if(j == capacity){
			n++;
			j = 0;
			for(l = 0; l < 127; l++){
				occ[l][n] += occ[l][n-1];
			}
		}
		//occ table construction
		occ[alpha][n]++;
		// ctable construction
		ct[alpha]++;
		cTable[alpha]++;
		j++;
	}
	// accumulate the cTable
	j = 0;
	for(i = 1; i < 127; i++){
		if(cTable[i] > 0){	
			cTable[i] += cTable[j];
			j = i;
		}
		
	}

	fseek(bwtf, 0, SEEK_SET);
	return occ;
}

//In this function, it can recogenize the index is in the top half or bottom half in it's block
int fileOccFunction(int** occ, int index, int capacity, FILE* bwtf, char searchWord){
	int ith = 0;
	int i;
	int nthBlock = index/capacity;
	char alpha;
	if(searchWord == Deliminater){
		searchWord = 0;
	}
	if((index - nthBlock*capacity) <= capacity/2){//in the up half
		if(nthBlock-1 >= 0){//fixed the bug which should be ">="
			ith = occ[searchWord][nthBlock-1];
		}
		for(i = 0; i <= capacity/2; i++){
			fseek(bwtf, nthBlock*capacity+i, SEEK_SET);
			alpha = fgetc(bwtf);
			if(alpha == Deliminater){
				alpha = 0;
			}
			if(alpha == searchWord){
				ith++;
			}
			if(nthBlock*capacity+i == index){	
				return ith;
			}
		}
	}
	else{//in the bottom half
		ith = occ[searchWord][nthBlock];

		for(i = 0; i <= capacity/2; i++){
			fseek(bwtf, ((nthBlock+1)*capacity-1)-i, SEEK_SET);
			alpha = fgetc(bwtf);
			if(alpha == Deliminater){
				alpha = 0;
			}

			if(alpha == searchWord){
				ith--;
			}
			if(((nthBlock+1)*capacity-1)-i == index){
				return ith+1;
			}
			
		}
	}	
}
//------------------------------------------------------end of common part---------------------------------------------------

//---------------------------------------------------start "-m" search part--------------------------------------------------
/*******************************************************************************************
     *
     * This is the "-m" search part, which olny search the number query string. 
     * And for this part, the program does not care about the duplicates.
     * 
 *******************************************************************************************/
int upToDown(char prevWord, int** occ, int* cTable, int* ct, int capacity, FILE* bwtf, int start, int end){
	int i = start;
	int prevStart;// previous word start position in the occ table
	char alpha;
	int occIndex;
	while(i < end){
		fseek(bwtf, i, SEEK_SET);
		alpha = fgetc(bwtf);
		if(alpha == Deliminater){
			alpha = 0;
		}
		if(alpha == prevWord){
			occIndex = fileOccFunction(occ, i, capacity, bwtf, alpha);
			return (cTable[prevWord] - ct[prevWord]) + occIndex - 1;
		}
		i++;
	}	
	return -1;
}


int downToUp(char prevWord, int** occ, int* cTable, int* ct, int capacity, FILE* bwtf, int start, int end){//int fIndex, 
	int i = end;
	int prevEnd;
	char alpha;
	int occIndex;
	while(i >= start){
		fseek(bwtf, i, SEEK_SET);
		alpha = fgetc(bwtf);
		if(alpha == Deliminater){
			alpha = 0;
		}
		if(alpha == prevWord){
			occIndex = fileOccFunction(occ, i, capacity, bwtf, alpha);
			return (cTable[prevWord] - ct[prevWord]) + occIndex - 1;
		}
		i--;
	}
	return -1;
}


void searchM(int** occ, int* cTable, int* ct, int capacity, FILE* bwtf, char deliminater, char* string){//true
	int len = strlen(string);
	int endIndex = len - 1;
	int len2 = 0;
	char prevWord = string[endIndex];
	if(prevWord == deliminater){
		prevWord = 0;
	}
	if(len == 1){
		printf("%d\n", ct[prevWord]);
		return;
	}

	int end = cTable[prevWord];
	int last = end - 1;//the last index of the backWord
	int first = cTable[prevWord] - ct[prevWord];

	int prevFirst, prevLast;
	while(first < last && endIndex >= 1){
		// if "first < 0" or "first < 0", there is no match words
		if(first < 0 || first < 0){
			break;
		}

		prevWord = string[--endIndex];
		prevFirst = upToDown(prevWord, occ, cTable, ct, capacity, bwtf, first, last);
		prevLast = downToUp(prevWord, occ, cTable, ct, capacity, bwtf, first, last);
		first = prevFirst;
		last = prevLast;
	}
	if(last < first || first < 0 || first < 0){
		printf("0\n");
	}
	else{
		printf("%d\n", last - first + 1);
	}
}
//---------------------------------------------------end "-m" search part--------------------------------------------------
//----------------------------------------------start "-n" and "-a" search part--------------------------------------------
/***************************************************************************************************
     *
     * This is the "-n" and "-a" search part, which not olny search the number query string, 
     * but also cancel the duplicates. We were using the Linked List to store each subsentences'
     * identify, which matched the query string. And we only store those identifies by once, 
     * according to the acsend order.   
     * 
 **************************************************************************************************/
char* searchN_A(int** occ, int* cTable, int* ct, int capacity, FILE* bwtf, char deliminater, char* string, FILE* indexf){	
	int len = strlen(string);
	int endIndex = len - 1;
	int len2;
	char backWord = string[endIndex];
	if(backWord == deliminater){
		backWord = 0;
	}
	int i;
	char alpha;
	//"occIndex" is used to show this peticular alphabet's ordinal of the bwt of this kind of word
	//"index" is used to show the ordinal in the suffix array(First column)
	int occIndex, index;
	int flag;// "flag" is used to one sentence add one time search string

	/* this is the deliminater's occ index in the occ table, 
	 * then store it in the linked list to help us find the duplicate*/
	int delimOccIndex = 0;
	int delimIndex = 0;
	int count = 0;//"count" is used to count the number of search string

	/* Get the number of bytes of the delimiter covered
	 *  And becuase we start from 1 to count, then we need count 1 more bit space*/
	int delNum = iceil((ct[0]+1), 8);
	char* delId = (char*) calloc(delNum, sizeof(char));

	char prevWord = string[endIndex];
	if(prevWord == deliminater){
		prevWord = 0;
	}

	int end = cTable[prevWord];
	int last = end - 1;//the last index of the backWord
	int first = cTable[prevWord] - ct[prevWord];
	
	int prevFirst, prevLast;
	while(first < last && endIndex >= 1){
		prevWord = string[--endIndex];
		prevFirst = upToDown(prevWord, occ, cTable, ct, capacity, bwtf, first, last);
		prevLast = downToUp(prevWord, occ, cTable, ct, capacity, bwtf, first, last);
		first = prevFirst;
		last = prevLast;
	}
	if(last < first || first < 0 || first < 0){
		return delId;
	}
	else{
		i = first;
		end = last;
		while(i <= end){
			fseek(bwtf, i, SEEK_SET);
			alpha = fgetc(bwtf);
			if(alpha == deliminater){
				alpha = 0;
			}
			occIndex = fileOccFunction(occ, i, capacity, bwtf, alpha);
			if(alpha == 0){
			  fseek(indexf, 4*(occIndex-1), SEEK_SET);
			  occIndex = getw(indexf);
			  index = occIndex;
			}
			else{
			  index = (cTable[alpha] - ct[alpha]) + occIndex - 1;
			}
			
			if(alpha == 0){
			  delimOccIndex = index;
			  /* if "delimOccIndex" shows the last ordinal, 
			   * which means the string belone first sentence
			   * Then add node number is "0"
			   */
			  if(delimOccIndex == ct[0]-1){
			    delimOccIndex = 1;//delimOccIndex = 0;
			  }
			  else if(delimOccIndex != -1){
			    delimOccIndex += 2;//delimOccIndex++;
			  }
			  
			  bitWriter(delId, delimOccIndex);
			  i++;
			  continue;
			}
			while(1){

				//if arrive the deliminater, add its occ index into linked list and break this leave's while
				if(alpha == 0){
					delimOccIndex = index;
					/* if "delimOccIndex" shows the last ordinal, 
					 * which means the string belone first sentence
					 * Then add node number is "0"
					 */
					if(delimOccIndex == ct[0]-1){
						delimOccIndex = 1;//delimOccIndex = 0;
					}
					else if(delimOccIndex != -1){
						delimOccIndex += 2;//delimOccIndex++;
					}
					bitWriter(delId, delimOccIndex);
					break;
				}
				fseek(bwtf, index, SEEK_SET);
				alpha = fgetc(bwtf);
				if(alpha == deliminater){
					alpha = 0;
				}
				occIndex = fileOccFunction(occ, index, capacity, bwtf, alpha);
				/* if alpha is deliminater, fetch it's First Column order as occ table order,
				 * else, not change
				 */
				if(alpha == 0){
					fseek(indexf, 4*(occIndex-1), SEEK_SET);
					occIndex = getw(indexf);
					index = occIndex;
				}
				else{
					index = (cTable[alpha] - ct[alpha]) + occIndex - 1;
				}
			}
			i++;
		}
	}
	return delId;
}

void searchN(int** occ, int* cTable, int* ct, int capacity, FILE* bwtf, char deliminater, char* string, FILE* indexf){
	int count = 0;
	int i;
	char* delId;	
	delId = searchN_A(occ, cTable, ct, capacity, bwtf, deliminater, string, indexf);

	for(i = 1; i <= ct[0]; i++){
		if(bitreader(delId, i) > 0){
			count++;
		}
	}
	printf("%d\n", count);
	free(delId);
}


void searchA(int** occ, int* cTable, int* ct, int capacity, FILE* bwtf, char deliminater, char* string, FILE* indexf){
	int i;
	char* delId;	
	delId = searchN_A(occ, cTable, ct, capacity, bwtf, deliminater, string, indexf);

	for(i = 1; i <= ct[0]; i++){
		if(bitreader(delId, i) > 0){
			printf("%d\n", i);
		}
	}
	free(delId);
}
//---------------------------------------------------end "-n"  and "-a" search part---------------------------------------------
//-------------------------------------------------------start "-i" search part-------------------------------------------------
/***************************************************************************************************
     *
     * This is the "-i" search part, which input the identies "i" and "j", we only need print out 
     * the context from the ith sentence to jth sentence.   
     * 
 **************************************************************************************************/
void searchI(int** occ, int* cTable, int* ct, int capacity, FILE* bwtf, char deliminater, int start, int end, FILE* indexf, char* sentenceArray){
	int i = start - 1;
	int j = 0; // count each sentence's character
	char alpha;
	//"occIndex" is used to show this peticular alphabet's ordinal of the bwt of this kind of word
	//"index" is used to show the ordinal in the suffix array(First column)
	int occIndex, index;
	/* this is the deliminater's occ index in the occ table, 
	 * then store it in the linked list to help us find the duplicate*/
	int delimOccIndex = 0;
	//"count" is used to count the number of search string
	int count = 0;
	while(i < end){
		fseek(bwtf, i, SEEK_SET);
		alpha = fgetc(bwtf);
		if(alpha == deliminater){
			alpha = 0;
		}
		occIndex = fileOccFunction(occ, i, capacity, bwtf, alpha);
		index = (cTable[alpha] - ct[alpha]) + occIndex - 1;
		while(1){
			//if arrive the deliminater, add its occ index into linked list and break this leave's while
			if(alpha == 0){
				break;
			}
			else{
				sentenceArray[j] = alpha;
				j++;
			}
			fseek(bwtf, index, SEEK_SET);
			alpha = fgetc(bwtf);
			if(alpha == deliminater){
				alpha = 0;
			}
			occIndex = fileOccFunction(occ, index, capacity, bwtf, alpha);

			/* if alpha is deliminater, fetch it's First Column order as occ table order,
			 * else, not change
			 */
			if(alpha == 0){
				fseek(indexf, 4*(occIndex-1), SEEK_SET);
				occIndex = getw(indexf);
				index = occIndex;
			}
			else{
				index = (cTable[alpha] - ct[alpha]) + occIndex - 1;
			}
		}
		while(j>0){
			printf("%c",sentenceArray[j-1]);
			j--;
		}
		printf("\n");
		i++;
	}
	
}
//------------------------------------------------------------end "i" search part-----------------------------------------------

char* giveName(char* s){
	int len = strlen(s);
	int i = 0, count = 0;
	int count2 = 0, j = 0;
	char name[20];
	for(i = 0; i < len; i++){
		if(s[i] == '/'){
			count++;
		}
	}
	for(i = 0; i < len; i++){
		if(s[i] == '/'){
			count2++;
		}
		if(count2 == count && s[i] != '/'){
			if(s[i] == '.' ){
				break;
			}
			name[j] = s[i];
			j++;
		}
	}
	char* fileName = calloc((j+1),sizeof(char));
	strcpy(fileName, name); 
	fileName[j] = '\0';
	return fileName;
}


char* combineName(char* frontName, char* tailName){
	int flen = strlen(frontName);
	int tlen = strlen(tailName);
	int len = flen + tlen;
	int i;
	char* fullName = (char*)calloc(len+1, sizeof(char));
	strcat(fullName, frontName);
	strcat(fullName, tailName);
	fullName[len] = '\0';
	return fullName;
}


int readFile(FILE* bwtf, char deliminater){//(char* bwtFile, char* indexFile, char deliminater){	
	fseek(bwtf, 0, SEEK_END);
	int fsize = ftell(bwtf);
	fseek(bwtf, 0, SEEK_SET);
	Deliminater = deliminater;
	return fsize;
}

void writecTableFile(int* ct, int* cTable, FILE* ctf){
	fwrite(ct, sizeof(int), 127, ctf);	
	fwrite(cTable, sizeof(int), 127, ctf);
}

void writeOccFile(int** occ, FILE* occf, int fsize, int capacity){
	int nblocks = fsize/capacity;
	if(nblocks*capacity < fsize){
		nblocks++;
	}

	int i;
	for(i = 0; i < 127; i++){
		fwrite(occ[i], sizeof(int), nblocks, occf);
	}
}

void readcTableFile(int* ct, int* cTable, FILE* ctf){
	fread(ct, sizeof(int), 127, ctf);
	fread(cTable, sizeof(int), 127, ctf);
}

// This is stored the occ table into the memory
int** readOccFile(FILE* occf, int fsize, int capacity){
	int nblocks = 0;
	int i;
	nblocks = fsize/capacity;
	if(nblocks*capacity < fsize){
		nblocks++;
	}

	int** occ = (int**) calloc(127, sizeof(int*));
	for(i = 0; i < 127; i++){
		occ[i] = (int*) calloc(nblocks, sizeof(int));
		fread(occ[i], sizeof(int), nblocks, occf);
	}
	return occ;
}


int* digitalRecog(char* str){
	int len = strlen(str);
	int i = 0;
	int first = 0;
	int* arr = calloc(2, sizeof(int));
	int n = 0;

	for(i = 0; i < len; i++){
		for (arr[n] = str[i++] - '0'; isdigit(str[i]); i++) {
        	        arr[n] = arr[n] * 10 + str[i] - '0';
		}
		n++;
	}
	return arr;
}


int main(int arg, char** argv){
	char delimiter;
	char* inputFile = argv[2];
	char* frontName = giveName(inputFile);//get the specific bwt file name
	
	// this part is used to give each bwt file specific occtable file and c table file name
	char* occTailName = "_occ.txt";
	char* ctableTailName = "_cT.txt";
	char* ctableFile = combineName(frontName, ctableTailName);
	char* occFile = combineName(frontName, occTailName);
	
	// this part is used to get the occ table and c table store path
	char* indexPath = argv[3];
	char* searchPath = combineName(indexPath, "/");
	char* occPath = combineName(searchPath, occFile);
	char* ctablePath = combineName(searchPath, ctableFile);

	char* indexTail = "_index.aux";
	char* indexFile = combineName(frontName, indexTail);
	
	FILE* occf;
	FILE* ctf;
	int** occ;
	int ct[127] = {0};
	int cTable[127] = {0};
	int capacity;
	int fsize;
	FILE* bwtf = fopen(argv[2], "rb");//arg[2], which is the file name; argv[1], which is delimiter
	FILE* indexf = fopen(indexFile, "rb");
	
	// this part is used to process the delimiter equal to the "\n" or "\t" 
	if(strcmp(argv[1], "\\n")==0){
		delimiter = '\n';
	}
	else if(strcmp(argv[1], "\\t")==0){
		delimiter = '\t';
	}
	else if(strcmp(argv[1], "\\")==0){
		delimiter = 92;
	}
	else{
		delimiter = argv[1][0];
	}

	fsize = readFile(bwtf, delimiter);
	// if file size is bigger than threshold, write occ table and c table into the index folder
	if(fsize > FILE_SIZE_THRESHOLD){
		capacity = CAPACITY_SIZE_FOR_LARGE_FILE;// can we according to the file size to compute the capacity?
		if(occf = fopen(occPath, "rb")){// if two files existed
			if(ctf = fopen(ctablePath, "rb")){
				readcTableFile(ct, cTable, ctf);
				occ = readOccFile(occf, fsize, capacity);
				fclose(ctf);
				fclose(occf);

				if(strcmp(argv[4], "-m")==0){
					searchM(occ, cTable, ct, capacity, bwtf, delimiter, argv[5]);
				}
				else if(strcmp(argv[4], "-n")==0){
					searchN(occ, cTable, ct, capacity, bwtf, delimiter, argv[5], indexf);
				}
				else if(strcmp(argv[4], "-a")==0){

					searchA(occ, cTable, ct, capacity, bwtf, delimiter, argv[5], indexf);
				}
				else if(strcmp(argv[4], "-i")==0){
					char sentenceArray[5005] = {0};
					int* arr = digitalRecog(argv[5]);
					if(arr[0] <= 0 || arr[1] <= 0){
						return 0;
					}
					searchI(occ, cTable, ct, capacity, bwtf, delimiter, arr[0], arr[1], indexf, sentenceArray);
				}
				free(occ);
			}			
		}
		else{//else creat the files
			occ = fileoccTable(bwtf, fsize, delimiter, capacity, cTable, ct);
			ctf = fopen(ctablePath, "wb");
			occf = fopen(occPath, "wb");
			writecTableFile(ct, cTable, ctf);
			writeOccFile(occ, occf, fsize, capacity);
			fclose(ctf);
			fclose(occf);

			if(strcmp(argv[4], "-m")==0){
				searchM(occ, cTable, ct, capacity, bwtf, delimiter, argv[5]);
			}
			else if(strcmp(argv[4], "-n")==0){
				searchN(occ, cTable, ct, capacity, bwtf, delimiter, argv[5], indexf);
			}
			else if(strcmp(argv[4], "-a")==0){
				searchA(occ, cTable, ct, capacity, bwtf, delimiter, argv[5], indexf);
			}
			else if(strcmp(argv[4], "-i")==0){
				char sentenceArray[5005] = {0};
				int* arr = digitalRecog(argv[5]);
				if(arr[0] <= 0 || arr[1] <= 0){
					return 0;
				}
				searchI(occ, cTable, ct, capacity, bwtf, delimiter, arr[0], arr[1], indexf, sentenceArray);
			}
			free(occ);
		}
	}
	// else file less than threshold, the capacity become smaller, which we can make more blocks for the occ table
	else{

			capacity = CAPACITY_SIZE;
			occ = fileoccTable(bwtf, fsize, delimiter, capacity, cTable, ct);
			if(strcmp(argv[4], "-m")==0){
				searchM(occ, cTable, ct, capacity, bwtf, delimiter, argv[5]);
			}
			else if(strcmp(argv[4], "-n")==0){
				searchN(occ, cTable, ct, capacity, bwtf, delimiter, argv[5], indexf);
			}
			else if(strcmp(argv[4], "-a")==0){
				searchA(occ, cTable, ct, capacity, bwtf, delimiter, argv[5], indexf);
			}
			else if(strcmp(argv[4], "-i")==0){
				char sentenceArray[5005];
				int* arr = digitalRecog(argv[5]);
				if(arr[0] <= 0 || arr[1] <= 0){
					return 0;
				}
				searchI(occ, cTable, ct, capacity, bwtf, delimiter, arr[0], arr[1], indexf, sentenceArray);
			}
			free(occ);
	}
	fclose(bwtf);
	fclose(indexf);
	return 0;
}

