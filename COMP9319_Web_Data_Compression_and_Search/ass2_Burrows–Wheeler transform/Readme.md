********* I.bwtencode Part **********

In this part, I separated the suffix sorting by two part: delimiter sorting and other alphabets sorting.

Before the sorting part, I did pre-processing. Which was store the original file into the memory and scan through the hold file to count each alphabet's frequency and then according to these frequency to malloc memory to do the sorting part.

** First delimiter sorting: 
	Because of the search convenient, I fetched the delimiter in the original file first. And kept their orignial order to write the bwt word into the bwt file. Also, I kept each delimiter's original file index array in the memory(which could help me to write the position file).

** Second other alphabets sorting:
	
	Because the text file only " include any visible ASCII alphabets (i.e., any character with ASCII value from 32 to 126), tab (ASCII 9) and newline". The encode part only go through the Ascii code from 0 to 126 to put each Ascii character started suffixes into the "bucketProcess" function to do the sorting. In the "bucketProcess" function, sort the suffixes first. And then, according to the suffix array we got, minus 1 to get the bwt word. If the word is the delimiter, the function will entry the "delimOrder" function to compare "each delimiter's original file index array" to find it's index in the first column, and write it into the position file.

** sorting algorithm ** 
	In this part, I used the recursive quick sort to sort the suffix according to thire lexicographic order, and when the common prefix suffixes encounter the delimiter stop comparing. Becuase the quick sort may change the common prefix suffixes' orignial order. Then I put those kind of suffix into another quick sort, and sort them according to thire delimiter order.



********* II.bwtsearch Part **********

In this part, I constructed the occurrence table and First column table(C table) first. The occurrence table was used to keep check points which are storing the "capacity" number words's occurrence times.(And this occurrence table is 2D table, fist dimension's space is 127, and second dimension'space is fileSize/capacity). The First column table was used to keep the accumulation from the start to this particular alphas' number, and all aphalbets permutated by thire lexicographic order(except the delimiter, it's always on the top of the First column table).

** "-m" Search **
	In this part, I used the backword search knowledge by using the c table to find the query string's last word's first row and last row index first in the first column. Then combine the occurrence table and c table to find it's previous word's first row and last row etc. Finally, we can find the query sting's first word's range in the first column, the number we want to count is using the last row index to minus 
first row index. 

** "-n" and "-a" Search **
	In this part, I used a common function called "searchN_A" to put the delimiter identifies, which the sentences have the query string, into a bit map. And then search "-n" or search "-a" read this bit map individually to get the answer.
	In the "searchN_A" function, I used the method as the search "-m" first, which could help to find the range which fit the query string. And then do the back word search agin until encounter the delimiter, and write this sentence's id into the bitmap.

** "-i" Search **
	Because when I contructed the bwt file, I fetched the delimiter's suffixes first and stored bwt word by the orignial file occurrence order. So when I do the search "-i", I can find the delimiter's position in the first column directlly. And then do the back word search until encounter the other delimiter. Then print this sentence out.








