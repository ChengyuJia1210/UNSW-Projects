/*******************************************************
* This version is the third version, which is speliting
* each alphabet into it's own bucket, and each time only
* sort one bucket
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SMALL_SIZE 100000
#define INDEX_BUF_SIZE 10000


char Deliminater;

typedef struct {
	int index;
}rotation;



//---------------------------start sort part-----------------------------------------
/******************************************************************************
* This part is using three way radix quick sort, it has the radix's stable 
* character, and quick sort's nlogn speed
*
*******************************************************************************/

char charAt(int index, int row, char* buf){//charAt(rotation* arr, int row, char* buf){
	return buf[index + row];
}

void exchange(rotation* arr, int lt, int i){
	rotation temp;

	temp = arr[i];
	arr[i] = arr[lt];
	arr[lt] = temp;
}


void sort2(rotation* arr, int low, int high){
	if(low >= high)
		return;
	int i = low, j = high;
        int t = arr[i].index;
        while (j > i) {
            while (arr[j].index > t && j > i)
                j--;
		exchange(arr, i, j);
            while (arr[i].index <= t && j > i)
                i++;
		exchange(arr, i, j);
        }
	exchange(arr, i, j);

	if(low < high){
		sort2(arr, low, j-1);
		sort2(arr, j+1, high);
	}
}

void sort(rotation* arr, int l, int h, int c, char* buf){//c means column
	if(h <= l){
		return;
	}
	int lt = l, gt = h;
	char v = charAt(arr[l].index, c, buf);// the lowest level's character
//	int useForGt;

	if(v==Deliminater) {v=0;}//-1;}

	int i = l + 1;
	while(i <= gt){
		char t = charAt(arr[i].index, c, buf);//each row's character, used to compare the first row's character
		if(t==Deliminater) {t=0;}//-1;}
		if(t < v){
			exchange(arr, lt++, i++);
		}
		else if(t > v){
			exchange(arr, i, gt);
			gt--;	
		}
		else{
			i++;
		}
	}
	sort(arr, l, lt - 1, c, buf);
	if(v >= 0){//!= Deliminater){//assume right now the deliminater is the smallest one of the ascii 
		if(v > 0){
			sort(arr, lt, gt, c+1, buf);
		}
		else if(v == 0){
			sort2(arr, lt, gt);
		}
	}
	sort(arr, gt+1, h, c, buf);
}


//----------------------------end sort part------------------------------------------
rotation* assignIndex(char* buf, int numFreq, int fsize, char alphabet){
	int i;
	int j = 0;
	rotation* eachBucket = (rotation*) malloc(numFreq*sizeof(rotation));

	for(i = 0; i < fsize; i++){
		if(buf[i] == alphabet){
			eachBucket[j].index = i;
			j++;
		}
	}
	return eachBucket;
}
/***************************************************************************************
*
*
* This part is used to fetch the delimiter first. We don't need really sort it, only 
* fetch each deliminater index according to it's original order.
*
*
****************************************************************************************/
int* delimiterSort(FILE* outFile, char* buf, int fsize, int num, char deliminater){// only sort first column
	int i;
	int j = 0; 
	
	char*  delimStore = (char*) malloc(num*sizeof(char));
	int* returnDelim = (int*) malloc(num*sizeof(int));

	//sort delimitnaters by ofiginal file order
	for(i = 0; i < fsize; i++){
		if(buf[i] == deliminater){
			returnDelim[j] = i;
			if(i == 0){
				delimStore[j] = buf[fsize - 1];
			}
			else{
				delimStore[j] = buf[i -1];
			}
			j++;
			
		}
	}
	fwrite(delimStore, sizeof(char), num, outFile);
	free(delimStore);
	return returnDelim;
}



int delimOrder(int* returnDelim, int index, int numOfDeliminater){//find the last column's deliminater in the first column's deliminater's order
	int low = 0;
	int up = numOfDeliminater;// - 1;
	int mid = 0;

	//int count = 0;
	while(low <= up){
		
		mid = ((up+low)/2);//+low;
		if(index > returnDelim[mid]){
			low = mid+1;//mid;
		}
		else if(index < returnDelim[mid]){
			up = mid-1;//mid;
		}
		else if(index == returnDelim[mid]){
			break;
		}
		//count++;
	}
	return mid;
}

void bucketProcess(FILE* outFile, FILE* indexFile, char* buf, int numFreq, rotation* eachBucket, char* smallBuf, int fsize, int* indexBuf, int* returnDelim, int numOfDeliminater){
	int i;
	int j = 0;
	int n = 0;//count the deliminater number
	int delimIndex;
	sort(eachBucket, 0, numFreq - 1, 1, buf);


	for(i = 0; i < numFreq; i++){
		// this part for deliminater index
		if(n == INDEX_BUF_SIZE){
			fwrite(indexBuf, sizeof(int), n, indexFile);
			memset(indexBuf, 0, n);
			n = 0;
		}
		// thish part for bwt
		if(j == SMALL_SIZE){
			fwrite(smallBuf, sizeof(char), j, outFile);
			memset(smallBuf, 0, j);
			j = 0;
		}
		if(eachBucket[i].index == 0){
			smallBuf[j] = buf[fsize - 1];
			if(buf[fsize - 1] == Deliminater){
				delimIndex = delimOrder(returnDelim, fsize - 1, numOfDeliminater);
				indexBuf[n] = delimIndex;
				n++;
			}
		}
		else{
			smallBuf[j] = buf[eachBucket[i].index - 1];
			if(buf[eachBucket[i].index - 1] == Deliminater){
				delimIndex = delimOrder(returnDelim, eachBucket[i].index - 1, numOfDeliminater);
				indexBuf[n] = delimIndex;
				n++;
			}
		}
		j++;
	}	
	fwrite(smallBuf, sizeof(char), j, outFile);
	memset(smallBuf, 0, j);
	j = 0;
	
	fwrite(indexBuf, sizeof(int), n, indexFile);
	memset(indexBuf, 0, n);
	n = 0;
}

void encode(char deliminater, char* inputFile, char* outputFile, char* indexFile){
	int fsize;
	char* buf;
	int freq[127] = {0};
	char smallBuf[SMALL_SIZE] = {0};
	int indexBuf[INDEX_BUF_SIZE] = {0};
	int i, numFreq;
	char alphabet;
	rotation* eachBucket;
	int* deliminaterOrder;
	
	FILE* inf = fopen(inputFile, "rb");
	fseek(inf, 0, SEEK_END);
	fsize = ftell(inf);
	fseek(inf, 0, SEEK_SET);
	buf = (char*) malloc(fsize*sizeof(char));
	fread(buf, fsize, 1, inf);
	fclose(inf);


	FILE* outf = fopen(outputFile, "awb");
	FILE* indexf = fopen(indexFile, "awb");

	for(i = 0; i < fsize; i++){
		freq[buf[i]]++;
	}
	// first process the deliminater suffix
	deliminaterOrder = delimiterSort(outf, buf, fsize, freq[deliminater], deliminater);
	// second the other alphabet
	for(i = 0; i < 127; i++){
		if(freq[i]>0 && i != deliminater){
			numFreq = freq[i];
			alphabet = i;
			eachBucket = assignIndex(buf, numFreq, fsize, alphabet);
			bucketProcess(outf, indexf, buf, numFreq, eachBucket, smallBuf, fsize, indexBuf, deliminaterOrder, freq[deliminater]);
			free(eachBucket);
		}
	}
	fclose(outf);
	fclose(indexf);
}


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
	char* fileName = calloc(j+1,sizeof(char));
	strcpy(fileName, name); 
	fileName[j] = '\0';
	return fileName;
}


char* combineName(char* frontName, char* tailName){
	int len = strlen(frontName)+strlen(tailName);
	char* fullName = calloc(len+1, sizeof(char));
	strcat(fullName, frontName);
	strcat(fullName, tailName);
	fullName[len] = '\0';
	return fullName;
}


int main(int argc, char** argv){
	char delimiter;
	char* tempFile = argv[2];
	char* inputFile = argv[3];
	char* outputFile = argv[4];
	char* frontName = giveName(outputFile);
	char* tailName = "_index.aux";
	char* indexFile = combineName(frontName, tailName);
	
	if(strcmp(argv[1], "\\n")==0){
		delimiter = '\n';
	}
	else if(strcmp(argv[1], "\\t")==0){
		delimiter = '\t';		
	}
	else{
		delimiter = argv[1][0];
	}
	Deliminater = delimiter;
	encode(delimiter, inputFile, outputFile, indexFile);
	return 0;
}
