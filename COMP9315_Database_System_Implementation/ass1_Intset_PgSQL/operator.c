/*    operator.c
 *
 ******************************************************************
 * This file is used for the PostgreSQL customized Operators. 
 * The functions use the intermediate intSet elements to do the processing.
 * Then, we can use the PostgreSQL manipulation to do some Query according to these functions.
 ******************************************************************
 */






#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int removeDuplicate(int *arr, int arr_len);
int cmpElement(const void*a,const void*b); 

//i <@ S, intSet S contains the integer i; that is, i belong S
int belone(int i, int* intSet, int len){
	int e = 0;
	for(int e = 0; e < len; e++){
		if(i == intSet[e]){
			return 1;
		}
	}
	return 0;
}

int intlen(int* arr){
	int a[10];
	return sizeof(arr)/sizeof(int);
}

//A @> B, does intSet A contain only values in intSet B? that is, for every element of A, is it an element of B? (A⊂B)
int truthSubset(int* A, int* B, int lenA, int lenB){
	int i = 0;//i is A's index
	int j = 0;//j is B's index
	int count = 0;//count the number that A's element equal to B's element

	
	for(i = 0; j < lenB; j++){
		for(i = 0; i < lenA; i++){
			if(B[j] == A[i]){
				count++;
			}
		}
	}
	if(count == i){
		return 1;
	}
	return 0;
}


/*A = B, intSets A and B are equal; that is, intSet A contains 
all the values of intSet B and intSet B contains all the values of intSet A, 
or, every element in A can be found in B, and vice versa.*/
int equal(int* A, int* B, int lenA, int lenB){
	int i;	// A's index
	int j;	// B's index
	int count = 0; // count the number of equivalent elements

	if(lenA != lenB){
		return 0;
	}
	for(i = 0; i < lenA; i++){
		for(j = 0; j < lenB; j++){
			if(A[i] == B[j]){
				count++;
			}		
		}
	}
	if(count == i){
		return 1;
	}
	return 0;
}


//A && B, takes the set intersection, and produces an intSet containing the elements common to A and B; that is, A∩B.
int* intersection(int* A, int* B, int lenA, int lenB){
	int i;	// A's index
	int j;	// B's index
	int* n = NULL; // meanless pointor, just use for realloc memory space
	int* inters = realloc(n, sizeof(int)); 
	int count = 0; //count commen digits

	for(i = 0; i < lenA; i++){
		for(j = 0; j < lenB; j++){
			if(A[i] == B[j]){
				inters[count] = A[i];
				count++;		
			}
		}
	}
	qsort(inters, count, sizeof(int), cmpElement);
	return inters;
}


//A || B,takes the set union, and produces an intSet containing all the elements of A and B; that is, A∪B.
int removeDuplicate(int *arr, int arr_len) {
    int i, j, k;  // index used for triple nested loop
    for (i = 0; i < arr_len; i++) {
        for (j = i + 1; j < arr_len;) {          // check next position
            if (arr[j] == arr[i]) {
                for (k = j; k < arr_len; k++) {  // compact original array
                    arr[k] = arr[k + 1];
                }
                arr_len --;
            } else {
                j ++;
            }
        }
    }
    return arr_len;
}

int cmpElement(const void*a,const void*b) {
    return *(int *)a - *(int *)b;
}

int* uni(int* A, int* B, int lenA, int lenB){
	int i;	// A's index
	int j;	// B's index
	int* n = NULL;// meanless pointor, just use for realloc memory space
	int* AunionB = realloc(n, sizeof(int));;
	int count = 0; //count diffrent digits
	int AunionB_len = 0; // this is used to store the return value after processed the "removeDuplicate()" function
	
	//AunionB = A;
	for(i = 0; i < lenA; i++){
		AunionB[count] = A[i];
		count++;
		for(j = 0; j < lenB; j++){
			if(A[i] != B[j]){
				AunionB[count] = B[j];
				count++;
			}
		}
	
	}

	AunionB_len = removeDuplicate(AunionB, count);
	qsort(AunionB, AunionB_len, sizeof(int), cmpElement);
	return AunionB;

}

/*A !! B, takes the set disjunction, and produces an intSet containing elements 
that are in A and not in B, or that are in B and not in A.*/
int* disjunction(int* A, int* B, int lenA, int lenB){
	int i;	// A's index
	int j;	// B's index
	int* n = NULL; // meanless pointor, just use for realloc memory space
	int* AdisjunctionB = realloc(n, sizeof(int));;
	int count = 0; //count diffrent digits
	int AunionB_len = 0;
	int flag = 0; // the flag is used to note A[i] and B[j] have commen element or not
	

	for(i = 0; i< lenA; i++){
		for(j = 0; j < lenB; j++){
			if(A[i] == B[j]){
				flag++;
				continue;
			}
		}
		if(flag == 0){
			AdisjunctionB[count] = A[i];
			count++;
		}
		flag = 0;
	}
	return AdisjunctionB;
}//this function is cotaining A's elements.




void main(){
	int i = 1;
	int intSetA[10] = {2,3,4,5,6,7,8,9,12, 1};
	int intSetB[10] = {1,2,3,4,5,6,7,8,95,121};//16,11,12,13,14,15};
	int lenA = sizeof(intSetA)/sizeof(intSetA[0]);
	int lenB = sizeof(intSetB)/sizeof(intSetB[0]);
	int answer;
	int* inters;
	int* AuniB;
	int* AdisjunctionB;
	int* BdisjunctionA;

	//answer = belone(i, intSetA);
	//answer = truthSubset(intSetA, intSetB, lenA, lenB);
	answer = equal(intSetA, intSetB, lenA, lenB);
	printf("%d\n\n", answer);

	inters = intersection(intSetA, intSetB, lenA, lenB);
	for(int i = 0; i< 8; i++)
		printf("intersectoin[%d]: %d\n", i, inters[i]);
	printf("\n\n");
	
	AuniB = uni(intSetA, intSetB, lenA, lenB);
	for(int i = 0; i < 11; i++)
		printf("AuniB[%d]:%d\n", i, AuniB[i]);
	printf("\n\n");

	AdisjunctionB = disjunction(intSetA, intSetB, lenA, lenB);
	for(int i = 0; i < 2; i++)
		printf("AdisjunctionB[%d]:%d\n", i, AdisjunctionB[i]);
	printf("\n\n");
	BdisjunctionA = disjunction(intSetB, intSetA, lenB, lenA);
	for(int i = 0; i < 2; i++)
		printf("BdisjunctionA[%d]:%d\n", i, BdisjunctionA[i]);

}

