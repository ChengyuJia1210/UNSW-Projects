#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <time.h>

typedef struct{
	/* positive number means internel node, the index shows the parent_index's index,
	 * negative number means leaf node, the index shows the leaf_index"s index*/
	int index;		
	unsigned  weight;
	int parent;
	int left_child;
	int right_child;
} node;


node* nodes = NULL;		//this is used to store whole tree nodes
int* leafnodes_index = NULL;	//this pointer is used to store the leaf nodes' index
int eof = 0; //when eof == 1, then the article arrive the END OF WORD, stop

unsigned int fre_buffer[256] = {0};	//used to store alphabets frequncy
long int all_size = 0; 		//store whole article alphabets' numbers
int active_elements = 0;	//store active alphabets in the ascii codes
int ele = 0;
unsigned char encode_buffer[512] = {0}; //256
int count_encode_buffer_position = 0;
unsigned char small_buf[1] = {0};//used to store 8bits binary modification
int count_binary_position = 0; //less than 8, this is used to count the position of modification, from left to right.
long int KMP_count = 0;//global variable

void initialize_tree(){
	nodes = (node*) calloc(2 * active_elements, sizeof(node));
	leafnodes_index = (int*) calloc(256, sizeof(int));// n leaf nodes
}


int add_node_into_tree(unsigned int weight, int index){
	int i = ele;
	ele++;
	//Reverse order to use insert node to compare the weight of the nodes, which existed in the "nodes"	
	while(nodes[i].weight > weight && i > 0){
		memcpy(&nodes[i+1], &nodes[i], sizeof(node));	
		if(nodes[i].index < 0){		//index<0 means its leaf node
			leafnodes_index[-nodes[i].index] += 1;
		}
		i--;
	}
	i++;
	nodes[i].index = index;
	nodes[i].weight = weight;
	if(index < 0){
		leafnodes_index[-index] = i;
	}
	else{
		return i;
	}
}

void construct_tree(){
	//printf("start construct tree\n");
	int i = 0, k = 1, l = 2;
	int index;
	while(i < 256){
		if(fre_buffer[i] > 0){
			add_node_into_tree(fre_buffer[i], -(i+1));	// negative index means leaf node, otherwise parent node
		}
		i++;
	}	
	while(l < (ele + 1)){//attention
		index = add_node_into_tree(nodes[k].weight + nodes[l].weight, l / 2);
		//for child nodes add parent node
		nodes[k].parent = index;
		nodes[l].parent = index;
		nodes[index].left_child = k;
		nodes[index].right_child = l;
		
		k += 2;
		l += 2;
	}
}



//---------------------------------------------first read file part----------------------------------------------------//
void read_Original_file(int bufsize, const char* filename, const char* outputfile){
	unsigned char* 	buf;
	int	inf;
	ssize_t	nread;
	
	buf = malloc(bufsize * sizeof(unsigned char));
	memset(buf, 0, sizeof(buf));

	inf = open(filename, O_RDONLY);


	int each=0;
	while((nread = read(inf, buf, bufsize)) != 0){
		int i = nread;
		int j = 0;
		while(i > j){//from the start index to read the buf
			each = buf[j];
			fre_buffer[each]++;	
			j++;
			all_size++;	
		}
	}
	free(buf);

	FILE *fd = fopen(outputfile,"wb");
	fwrite(fre_buffer, sizeof(unsigned int), 256, fd);
	fclose(fd);

	//count global vcariable "active_elements"
	int i = 0;
	while(i < 256){
		int num = fre_buffer[i];
		if(fre_buffer[i] > 0) { // count the number of the elements in 256 characters
			active_elements++;
		}
		i++;
	}
	close(nread);
}
//------------------------------------------ end of first read file part---------------------------------------------------//


//--------------------------------------------------encode part-----------------------------------------------------------//

void add_bit(char* bitarr, int k){   //left_to_right
	int i = k/8;
	int j = k%8;
	bitarr[i] = ((0x80 >> j) | bitarr[i]);
}


void bits_write(int bit, FILE* outputfile, int count_top_of_stack){ //,  unsigned char encode_buffer[], int count_top_of_stack
	if(count_binary_position < 8){
		if(bit){

			add_bit(small_buf, count_binary_position);
		}
		count_binary_position++;

		//if EOF, but small_buf does not full, still fwrit(), and clean small_buf and encode_buffer
		if(eof == 1 && count_top_of_stack == 0){
			memcpy(&encode_buffer[count_encode_buffer_position], &small_buf, sizeof(small_buf));
			fwrite(encode_buffer, sizeof(char), count_encode_buffer_position+1, outputfile);
			memset(small_buf, 0, sizeof(small_buf));
			memset(encode_buffer, 0, sizeof(encode_buffer));
			count_binary_position = 0;
			count_encode_buffer_position = 0;
			exit(0);
		}
	}
	else{
		count_binary_position = 0;
		memcpy(&encode_buffer[count_encode_buffer_position], &small_buf, sizeof(small_buf));
		memset(small_buf, 0, sizeof(small_buf));
		count_encode_buffer_position++;
		if(bit){
			add_bit(small_buf, count_binary_position);		
		}
		count_binary_position++;

		if(count_encode_buffer_position == 512 || (eof == 1 && count_top_of_stack == 0)){//256
			if(count_encode_buffer_position == 512){//256	
				fwrite(encode_buffer, sizeof(encode_buffer), 1, outputfile);
				memset(encode_buffer, 0, sizeof(encode_buffer));
				count_encode_buffer_position = 0;
			}
			else{
				fwrite(encode_buffer, sizeof(char), count_encode_buffer_position+1, outputfile);
				memset(encode_buffer, 0, sizeof(encode_buffer));
				count_encode_buffer_position = 0;
				exit(0);
			}
		}
	}

}


int encode(const char* inputfilename, const char* outputfile){//, const char* outputfile
	FILE* file = fopen(inputfilename, "rb");
	FILE* outfile = fopen(outputfile, "awb");//should be "awb"
//printf("entry encode\n");
	int alphabet;
	int count_alphabet = 0;
	int node_index;	//shows the leaf node's index of the "nodes"
	//int parent_index;
	int* stack = NULL;

	if(all_size == 0){
		eof = 1;
		return 0;
	}
	while((alphabet = fgetc(file))!=EOF){
		alphabet += 1;
		count_alphabet++;

		if(all_size == count_alphabet){
			eof = 1;
		}

		stack = (int*) calloc(active_elements, sizeof(int));
		int count_top_of_stack = 0;
		node_index = leafnodes_index[alphabet];


		stack[count_top_of_stack] = node_index%2;
		count_top_of_stack++;


		node_index = nodes[node_index].parent;

		while(node_index < 2 * active_elements-1){//attention
			stack[count_top_of_stack] = node_index%2;
			count_top_of_stack++;
			node_index = nodes[node_index].parent;
		}
		//write bit into the buffer
		count_top_of_stack--;
		while(count_top_of_stack >= 0){
			bits_write(stack[count_top_of_stack], outfile, count_top_of_stack);
			count_top_of_stack--;
		}
		free(stack);
	}
	fclose(file);
	fclose(outfile);
}


//----------------------------------------------------end of encode part-----------------------------------------------------------//

//------------------------------------------------------decode part---------------------------------------------------------------//
// when the position in the byte is 1,return 1; 0 otherwise
int bitreader(int pos, char* num){
	int new = 0;
	int i = pos/8;
	int j = pos%8;
	new = ((0x80 >> j) & num[i]) ;
	if(new != 0){
		return 1;
	}
	else{
		return 0;
	}
}


void decode(const char* inputfile, const char* outputputfile){
	FILE* infile = fopen(inputfile, "rb");
	FILE* ofile = fopen(outputputfile, "wb");
	fread(fre_buffer, sizeof(fre_buffer), 1, infile);//the pointer move to the 1024th poisition pointer
	
	int e1;
	for(e1 = 0; e1 < 256; e1++){
		if(fre_buffer[e1] > 0){
			active_elements++;
			all_size += fre_buffer[e1];
		}
	}
	initialize_tree();
	construct_tree();
	fclose(infile);

	/////////---------------------read file chunk by chunk------------------------------------/////////////
	unsigned char* 	buf;
	int	inf;
	ssize_t	nread;
	int bufsize = 10000000;
	long int count_all_size = 0;
	count_binary_position = 0;

	buf = (unsigned char*) malloc(bufsize * sizeof(unsigned char));
	memset(buf, 0, sizeof(buf));

	inf = open(inputfile, O_RDONLY);


	unsigned char store_decode_words_buf[256] = {0};
	int count_words = 0;
	int first = 0;	
	int root = ele;  //according to root to leaf, "ele" is root index in the "nodes" tree

	while((nread = read(inf, buf, bufsize)) != 0){

		if(first == 0){
			buf = buf + 1024;
			nread = nread -1024;
			first++;
		}
		if(nread == 0){
			exit(0);		
		}

		int i = nread;
		int j = 0;
		int decide = 0;

		while(i > j){//from the start index to read the buf
			while(count_binary_position < 8){
				decide = bitreader(count_binary_position, &buf[j]);						
				
				if(decide){ // decide == 1, then left side child
					root = nodes[root].left_child;
				}
				else{		// decide == 0; then right side child
					root = nodes[root].right_child;
				}
				if(nodes[root].index < 0){
						store_decode_words_buf[count_words] = -nodes[root].index - 1;
						count_all_size++;
						count_words++;
						root = ele;
				}

				if(count_all_size <= all_size){
					if(count_words == 256 || (j == i-1 && i < bufsize) || count_all_size == all_size){ //  || (j == i-1 && i < bufsize)
						if(count_all_size == all_size){
							fwrite(&store_decode_words_buf, count_words * sizeof(unsigned char), 1, ofile);
							count_words = 0;
							memset(store_decode_words_buf, 0, sizeof(store_decode_words_buf));
							exit(0);
						}
						if(count_words == 256){
							fwrite(&store_decode_words_buf, sizeof(store_decode_words_buf), 1, ofile);
							count_words = 0;
							memset(store_decode_words_buf, 0, sizeof(store_decode_words_buf));
						}
						else{
							fwrite(&store_decode_words_buf, count_words * sizeof(unsigned char), 1, ofile);
							count_words = 0;
							memset(store_decode_words_buf, 0, sizeof(store_decode_words_buf));
						}
					}
				}		
				count_binary_position++;				
			}
			count_binary_position = 0;
			j++;		
		}
	}
	//free(buf);
	close(nread);	
	fclose(ofile);
}
//----------------------------------------------end of decode part-------------------------------------------------//

//-------------------------------------------------KMP Part--------------------------------------------------------//
void prefixarray(char* parten, int* prefix, int n){
	if(n == 1){
		prefix[0] = 0;	
		return;
	}
	prefix[0] = 0;
	int len = 0;
	int i = 1;
	while(i < n){
		if(parten[len] == parten[i]){
			len++;
			prefix[i] = len;
			i++;
		}
		else{
			if(len > 0){
				len = prefix[len - 1];
			}
			else{
				prefix[i] = len;
				i++;	
			}
		}
	}
	int e = n - 1;
	while(e > 0){
		prefix[e] = prefix[e - 1];
		e--;
	}
	prefix[0] = -1;
}






int KMP(char* text, char* parten, int* prefix, int m, int n, int locat_parten_position){//m is text length, n is the parten length
	int i = locat_parten_position;//parten index
	int j = 0;//text index

	if(n == 1){
		while(j < m){	
			if(parten[i] == text[j]){\
				KMP_count++;
				j++;
			}
			else{
				j++;
			}
		}	
	}
	else{
		while(j < m){
			if(i == n - 1 && parten[i] == text[j]){
				KMP_count++;
				i = prefix[i];
				locat_parten_position = i;
			}
			if(parten[i] == text[j]){
				i++;
				j++;
				locat_parten_position = i;
			}
			else{
				i = prefix[i];
				if(i == -1){
					i++;
					j++;
				}		
				locat_parten_position = i;
			}
		}
	}
	return locat_parten_position;
}
//-----------------------------------------------end with KMP part-------------------------------------------------------//




//-----------------------------------------------decode and KMP search-------------------------------------------------------//
void KMPSearch(char* parten, const char* inputfile){//, const char* outputputfile
	unsigned char* 	buf;
	int	inf;
	ssize_t	nread;
	int bufsize = 10000000;
	long int count_all_size = 0;
	count_binary_position = 0;
	FILE* infile = fopen(inputfile, "rb");
	fread(fre_buffer, sizeof(fre_buffer), 1, infile);//the pointer move to the 1024th poisition pointer
	

	int e2;
	for(e2 = 0; e2 < 256; e2++){
		if(fre_buffer[e2] > 0){
			active_elements++;
			all_size += fre_buffer[e2];
		}
	}
	initialize_tree();
	construct_tree();
	fclose(infile);
	
	// count parten prefix array
	int parten_length = strlen(parten);//parten length which is prefix array length
	int prefix[parten_length];
	memset(prefix, 0, sizeof(prefix));
	prefixarray(parten, prefix, parten_length);
	int locat_parten_position = 0;

	/////////---------------------read file chunk by chunk------------------------------------/////////////
	buf = (unsigned char*) malloc(bufsize * sizeof(unsigned char));
	memset(buf, 0, sizeof(buf));
	inf = open(inputfile, O_RDONLY);// open file

	unsigned char store_decode_words_buf[256] = {0};//256
	int count_words = 0;
	int first = 0;		
	int root = ele;  //according to root to leaf, "ele" is root index in the "nodes" tree

	while((nread = read(inf, buf, bufsize)) != 0){
		if(first == 0){
			buf = buf + 1024;
			nread = nread -1024;
			first++;
		}
		if(nread == 0){
			exit(0);		
		}

		//printf("nread is %ld\n", nread);
		int i = nread;
		int j = 0;
		int decide = 0;
		while(i > j){//from the start index to read the buf
			while(count_binary_position < 8){
				decide = bitreader(count_binary_position, &buf[j]);				
				
				if(decide){ // decide == 1, then left side child
					root = nodes[root].left_child;
				}
				else{		// decide == 0; then right side child
					root = nodes[root].right_child;
				}
				if(nodes[root].index < 0){
						store_decode_words_buf[count_words] = -nodes[root].index - 1;
						count_all_size++;
						count_words++;
						root = ele;
				}

				//////////////////////////////////////////////////////
				if(count_all_size <= all_size){
					if(count_words == 256 || (j == i-1 && i < bufsize) || count_all_size == all_size){ //  || (j == i-1 && i < bufsize) 
						if(count_all_size == all_size){
							locat_parten_position = KMP(store_decode_words_buf, parten, prefix, count_words, parten_length, locat_parten_position);
							count_words = 0;
							memset(store_decode_words_buf, 0, sizeof(store_decode_words_buf));
							return;
						}
						else if(count_words == 256){

							locat_parten_position = KMP(store_decode_words_buf, parten, prefix, count_words, parten_length, locat_parten_position);
							count_words = 0;
							memset(store_decode_words_buf, 0, sizeof(store_decode_words_buf));
						}
						else{
							locat_parten_position = KMP(store_decode_words_buf, parten, prefix, count_words, parten_length, locat_parten_position);
							count_words = 0;
							memset(store_decode_words_buf, 0, sizeof(store_decode_words_buf));
						}
					}	
				}
				  
				count_binary_position++;				
			}
			count_binary_position = 0;
			j++;		
		}
	}
	//free(buf);
	close(nread);	
}

//-----------------------------------------------decode and KMP search-------------------------------------------------------//


void main(int arg, char** argv){
	if(strcmp(argv[1], "-e") == 0){
		read_Original_file(10000000, argv[2], argv[3]);
		initialize_tree();
		construct_tree();
		encode(argv[2], argv[3]);//argv[2] is file which want to be encoded, argv[3] is the ecoded bianry .huffman file
	}
	else if(strcmp(argv[1], "-d") == 0){
		decode(argv[2], argv[3]);//argv[2] is bianry .huffman file, argv[3] is the decoded original file
	}
	else if(strcmp(argv[1], "-s") == 0){
		KMPSearch(argv[2], argv[3]);//argv[2] is parten, argv[3] is huffman file
		printf("%ld\n", KMP_count);
	}
}


