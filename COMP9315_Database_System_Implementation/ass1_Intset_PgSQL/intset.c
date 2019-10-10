/*
 * src/tutorial/intset.c
 *
 ******************************************************************************
 This file contains routines that can be bound to a Postgres backend and
called by the backend in the process of processing queries.  The calling
format for these routines is dictated by Postgres architecture.
******************************************************************************/
#include "postgres.h"

#include "fmgr.h"
#include "libpq/pqformat.h" /* needed for send/recv functions */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PG_MODULE_MAGIC;

typedef struct _set {
    int32   length;   // does not work
    int32   real_len; // it works!!
    int     data[FLEXIBLE_ARRAY_MEMBER];
} intSet;

// Prototypes of functions
PG_FUNCTION_INFO_V1(intset_in);
PG_FUNCTION_INFO_V1(intset_out);

// Functions that are used in OPERATORS in PostgreSQL
PG_FUNCTION_INFO_V1(intset_isin);
PG_FUNCTION_INFO_V1(intset_cardinality);
PG_FUNCTION_INFO_V1(intset_subset);
PG_FUNCTION_INFO_V1(intset_equal);
PG_FUNCTION_INFO_V1(intset_intersection);
PG_FUNCTION_INFO_V1(intset_union);
PG_FUNCTION_INFO_V1(intset_disjunction);
PG_FUNCTION_INFO_V1(intset_difference);

// Actual implementation functions for intSet operating
static int intSet_ele_in_set(int ele, intSet *set);
static int isSubset(intSet *A, intSet *B);
static int isEqual(intSet *setA, intSet *setB);
intSet *doIntersection(intSet *A, intSet *B);
intSet *doUnion(intSet *A, intSet *B);
intSet *doDisjunction(intSet *A, intSet *B);
intSet *doDifference(intSet *A, intSet *B);

// Helper functions that are used to convert cstring to intset
intSet *convertToIntSet(int *tmp_arr, int count);
int removeDuplicate(int *arr, int arr_len); // function that is used to remove duplicate
int cmpElement(const void *a, const void *b); // compare function, useful in qsort()
char *convert(intSet *buffer);
intSet *getString(char *str);

/* 
 *  PG Magic funtions implementation
 */
Datum 
intset_in(PG_FUNCTION_ARGS) {
    char *str         = PG_GETARG_CSTRING(0);  // str points to the input string
    int  leftbracket  = 0;                     // count of leftbracket
    int  rightbracket = 0;                     // count of rightbracket
    int  i            = 0;                     // index of current character
    int  next         = 1;                     // index of next to the current character
    int  valid        = 1;                     // notice that 0 -> invalid, 1 -> valid
    int  num_ptr      = 0;                     // index of tmp_num_arr
    int  minus_flag   = 0;                     // flag used to assign negative sign to number
    int  comma_flag   = 0;                     // flag used to compare whether comma is valid

    unsigned long cur_num      = 0;            // current successive number
    long neg_cur_num = 0;
    int *tmp_num_arr; // temp array used to store all numbers

    tmp_num_arr = (int *)palloc(strlen(str) * VARHDRSZ);

    /*
    *  Validity checking of the input string
    */
    // check if the first char is '{'
    if (str[0] != 123) {
        valid = 0;
    }

    // iterating all characters in the str
    for (i = 0; str[i]; i++) {
        if (str[i] == 123) { // '{'
            leftbracket++;
        } else if (str[i] == 125) { // '}'
            if (comma_flag) {
                valid = 0;
                break;
            }
            rightbracket++;
        } else if (isdigit(str[i])) { // digit
            if (num_ptr > 0) {
                // tmp_num_arr is not empty, need to check invalid comma cases
                // e.g. {1 2}, {1 ,, 2}
                if (!comma_flag) {
                    valid = 0;
                    break;
                }
            }
            for (cur_num = str[i++] - '0'; isdigit(str[i]); i++) {
                cur_num = cur_num * 10 + str[i] - '0';
            } // have gotten a successive number

            if (cur_num > 2147483647) {
                if (minus_flag) {
                    if (cur_num > 2147483648UL) {
                        valid = 0;
                        break;
                    }
                } else {
                    valid = 0;
                    break;
                }
            }

            if (minus_flag) { // if it should be a negative number
                neg_cur_num = cur_num;
                minus_flag = 0;
                tmp_num_arr[num_ptr] = -neg_cur_num;
            } else {
                tmp_num_arr[num_ptr] = cur_num;
            }

            num_ptr++;
            comma_flag = 0;
            i--; // need push i back to the last index of this number
        } else if (isalpha(str[i])) { // character cannot be [A-Z] [a-z]
            valid = 0;
            break;
        } else if (str[i] == 45) { // character is '-'
            next = i + 1;
            if ((!isdigit(str[next])) || str[next] == 45) {
                valid = 0;
                break;
        }
            minus_flag = 1;
        } else if (str[i] == 44) { // charater is ','
            if (!comma_flag) {
                comma_flag = 1;
            } else {
                // case of successive commas but no digits
                // {1 ,, 2}
                valid = 0;
                break;
            }
        } else if (str[i] == ' ') { // ignore whitespace
            continue;
        } else { // any other invalid characters
            valid = 0;
            break;
        }
    }

    if ((leftbracket != 1) || (rightbracket != 1)) {
        valid = 0;
    }
    // END VALIDITY CHECKING

    if (valid == 0) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                        errmsg("invalid input syntax for intSet: \"%s\"", str)));
    } else {
        intSet *result;
        // store all efficient numbers into a new array
        int *num_arr = (int *)palloc(num_ptr * VARHDRSZ);

        for (i = 0; i < num_ptr; i++) {
            num_arr[i] = tmp_num_arr[i];
        }

        pfree(tmp_num_arr);
        num_ptr = removeDuplicate(num_arr, num_ptr); // after this, we get an array without duplicates.
        qsort(num_arr, num_ptr, VARHDRSZ, cmpElement); // call qsort() to sort the array


        result = (intSet *)palloc(VARHDRSZ + (num_ptr + 1) * VARHDRSZ);
        SET_VARSIZE(result, VARHDRSZ + (num_ptr + 1) * VARHDRSZ);
        memcpy(result->data, num_arr, num_ptr * VARHDRSZ);
        result->real_len = num_ptr;

        pfree(num_arr);
        PG_RETURN_POINTER(result);
    }
}

Datum
intset_out(PG_FUNCTION_ARGS) {
    intSet *buffer = (intSet *)PG_GETARG_POINTER(0);
    int i = 0;
    int *num_arr;
    int total_length = 1;
    char *final_str;
    char each_num[13]; // because each integer cannot exceed 2^31-1
    int lst_ch_idx;
    char *empty_str = "{}";  // when the set is empty.

    num_arr = buffer->data;

    for (; i < buffer->real_len; i++) {
        sprintf(each_num, "%d", num_arr[i]);
        total_length += strlen(each_num) + 1;
    }

    final_str = (char *)palloc(total_length * sizeof(char));
    strcpy(final_str, "{");
    for (i = 0; i < buffer->real_len; i++) {
        sprintf(each_num, "%d", num_arr[i]);
        strcat(final_str, each_num);
        strcat(final_str, ",");
    }

    // replace last ',' with '}'

    lst_ch_idx = strlen(final_str) - 1;
    final_str[lst_ch_idx] = '}';

    // for empty sets
    if (!lst_ch_idx) {
        pfree(final_str);
        PG_RETURN_CSTRING(empty_str);
    } else {
        PG_RETURN_CSTRING(final_str);
    }
}

// element <@ S, intSet S contains the integer element
Datum
intset_isin(PG_FUNCTION_ARGS) {
    int ele = (int) PG_GETARG_POINTER(0);
    intSet *set = (intSet *) PG_GETARG_POINTER(1);

    PG_RETURN_BOOL(intSet_ele_in_set(ele, set));
}

// @ s, Cardinality of s
Datum
intset_cardinality(PG_FUNCTION_ARGS) {
    intSet *s = (intSet *) PG_GETARG_POINTER(0);
    int setLen = s->real_len;

    PG_RETURN_INT32(setLen);
}

// A @> B, A is the subset of B
Datum
intset_subset(PG_FUNCTION_ARGS) {
    intSet *setA = (intSet *) PG_GETARG_POINTER(0);
    intSet *setB = (intSet *) PG_GETARG_POINTER(1);

    PG_RETURN_BOOL(isSubset(setA, setB));
}

// A = B, A equals to B
Datum
intset_equal(PG_FUNCTION_ARGS) {
    intSet *setA = (intSet *) PG_GETARG_POINTER(0);
    intSet *setB = (intSet *) PG_GETARG_POINTER(1);

    PG_RETURN_BOOL(isEqual(setA, setB));
}

// A && B, A∩B
Datum 
intset_intersection(PG_FUNCTION_ARGS) {
    intSet *setA = (intSet *) PG_GETARG_POINTER(0);
    intSet *setB = (intSet *) PG_GETARG_POINTER(1);
    intSet *result = doIntersection(setA, setB);

    PG_RETURN_POINTER(result);
}

// A || B, A∪B
Datum 
intset_union(PG_FUNCTION_ARGS) {
    intSet *setA = (intSet *) PG_GETARG_POINTER(0);
    intSet *setB = (intSet *) PG_GETARG_POINTER(1);
    intSet *result = doUnion(setA, setB);

    PG_RETURN_POINTER(result);
}

Datum 
intset_disjunction(PG_FUNCTION_ARGS) {
    intSet *setA = (intSet *) PG_GETARG_POINTER(0);
    intSet *setB = (intSet *) PG_GETARG_POINTER(1);
    intSet *result = doDisjunction(setA, setB);

    PG_RETURN_POINTER(result);
}

Datum 
intset_difference(PG_FUNCTION_ARGS) {
    intSet *setA = (intSet *) PG_GETARG_POINTER(0);
    intSet *setB = (intSet *) PG_GETARG_POINTER(1);
    intSet *result = doDifference(setA, setB);

    PG_RETURN_POINTER(result);
}

/*
 *  Implementation of actual operating functions
 */
static int 
intSet_ele_in_set(int ele, intSet *set) {
    int *arr = set->data;
    int setLen = set->real_len;
    int i = 0;
    
    for (; i < setLen; i++) {
        if (ele == arr[i]) { return 1; }
    }
    return 0;
}

static int 
isSubset(intSet *A, intSet *B) {
    int lenA = A->real_len;
    int lenB = B->real_len;
    int *dataA = A->data;
    int *dataB = B->data;

    int i = 0; // i traverse A
    int j = 0; // j traverse B
    int count = 0;

    for (; j < lenB; j++) {
        for (i = 0; i < lenA; i++) {
            if (dataB[j] == dataA[i]) { count++; }
        }
    }
    if (count == i) { return 1; }
    return 0;
}

static int
isEqual(intSet *setA, intSet *setB) {
    if (setA->real_len != setB->real_len) { return 0;}  // inequal length
    if (isSubset(setA, setB)) { return 1; }
    return 0;
}

// A && B, A∩B
intSet *doIntersection(intSet *A, intSet *B) {
    intSet *inters_output;
    int i; // A's index
    int j; // B's index

    int lenA = A->real_len;
    int lenB = B->real_len;
    int *arrA = A->data;
    int *arrB = B->data;

    int inters[lenA];
    int count = 0; // count commen digits

    memset(inters, 0, sizeof(inters));
    
    for (i = 0; i < lenA; i++) {
        for (j = 0; j < lenB; j++) {
            if (arrA[i] == arrB[j]) {
                inters[count++] = arrA[i];
            }
        }
    }

    inters_output = (intSet *) palloc(VARHDRSZ + (count + 1) * VARHDRSZ);
    SET_VARSIZE(inters_output, VARHDRSZ + (count + 1) * VARHDRSZ);
    memcpy(inters_output->data, inters, count * VARHDRSZ);
    inters_output->real_len = count;
    return inters_output;
}

// A || B, A∪B
intSet *doUnion(intSet *A, intSet *B) {
    intSet *union_output;
    int i; // A's index
    int j; // B's index

    int flag;

    int count;     // count diffrent digits
    int new_count;
    // int AunionB;
    int lenA = A->real_len;
    int lenB = B->real_len;
    int *arrA = A->data;
    int *arrB = B->data;


    int AunionB[lenA + lenB];
    // AunionB = palloc((lenA + lenB) *  VARHDRSZ);

    count = 0;

    // AunionB = A;
    for (i = 0; i < lenA; i++) {
        AunionB[count] = arrA[i];
        count++;
    }

    new_count = count;
    flag = 0;
    for (j = 0; j < lenB; j++) {
        for (i = 0; i < new_count; i++) {
            if (arrB[j] == AunionB[i]) {
                flag++;
            }
        }
        if (flag == 0) {
            AunionB[count++] = arrB[j];
        }
        flag = 0;
    }

    union_output = convertToIntSet(AunionB, count);

    return union_output;
}

// A !! B
intSet *doDisjunction(intSet *A, intSet *B) {
    intSet *disjunction_output;
    int i, j;
    int lenA = A->real_len;
    int lenB = B->real_len;
    int *arrA = A->data;
    int *arrB = B->data;

    int flag = 0;
    // int* n = NULL;// meanless pointor, just use for realloc memory space
    // int* AdisjunctionB = realloc(n, VARHDRSZ);
    int AdisjunctionB[lenA + lenB];
    int count = 0;

    if (lenA != 0) {
        if (lenB != 0) {
            for (i = 0; i < lenA; i++) {
                for (j = 0; j < lenB; j++) {
                    if (arrA[i] == arrB[j]) {
                        flag++;
                    }
                }
                if (flag == 0) {
                    AdisjunctionB[count++] = arrA[i];
                }
                flag = 0;
            }

            for (i = 0; i < lenB; i++) {
                for (j = 0; j < lenA; j++) {
                    if (arrB[i] == arrA[j]) {
                        flag++;
                    }
                }
                if (flag == 0) {
                    AdisjunctionB[count++] = arrB[i];
                }
                flag = 0;
            }

            disjunction_output = convertToIntSet(AdisjunctionB, count);
            
            return disjunction_output;
        } else {
            return A;
        }
    } else {
        return B;
    }
}

// A - B
intSet *doDifference(intSet *A, intSet *B) {
    intSet *difference_output;
    int i; // A's index
    int j; // B's index
    int lenA = A->real_len;
    int lenB = B->real_len;
    int *arrA = A->data;
    int *arrB = B->data;

    int AdiffrentB[lenA];
    int count = 0; // count diffrent digits
    int flag = 0; // the flag is used to note A[i] and B[j] have commen element or not

    for (i = 0; i < lenA; i++) {
        for (j = 0; j < lenB; j++) {
            if (arrA[i] == arrB[j]) {
                flag++;
                continue;
            }
        }
        if (flag == 0) {
            AdiffrentB[count] = arrA[i];
            count++;
        }
        flag = 0;
    }

    difference_output = convertToIntSet(AdiffrentB, count);
    return difference_output;
} // this function is cotaining A's elements.

/*
 *  Helper functions implementation
 */
intSet *convertToIntSet(int *tmp_arr, int count) {
    int *arr = palloc(count * VARHDRSZ);
    intSet *result;
    memcpy(arr, tmp_arr, count * VARHDRSZ);
    qsort(arr, count, VARHDRSZ, cmpElement);

    result = (intSet * ) palloc(VARHDRSZ + (count + 1) * VARHDRSZ);
    SET_VARSIZE(result, VARHDRSZ + (count + 1) * VARHDRSZ);
    memcpy(result->data, arr, count * VARHDRSZ);
    result->real_len = count;

    return result;
}

int removeDuplicate(int *arr, int arr_len) {
    int i, j, k; // index used for triple nested loop
    for (i = 0; i < arr_len; i++) {
        for (j = i + 1; j < arr_len;) { // check next position
            if (arr[j] == arr[i]) {
                for (k = j; k < arr_len; k++) { // compact original array
                    arr[k] = arr[k + 1];
                }
                arr_len--;
            } else {
                j++;
            }
        }
    }
    return arr_len;
}

int cmpElement(const void *a, const void *b) { return *(int *)a - *(int *)b; }
