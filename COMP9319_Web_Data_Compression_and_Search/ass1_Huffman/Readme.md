This part of code is using huffman algorithm to do the data compression. First 1024 bytes are using for statistics. And following context are the compressed data.
This part of codes can compress every binary code file. Please feel free to use this code.

**Compile:**
  make(Under the target path)
  

Explain the code:
1. The header just store the characters frequency for all 256 ascii codes

2. Encoding part reads orignial file 2 times. First time reading, store limited data quantity into limited memory space, and count the ascii codes frequency, after seval iterations, all the file context will be finished from scaning, then store this information into ".huffman" header. And then according to the word frequency to construct huffman tree. Second time reading, through each character and according to the huffman tree to get this character's huffman code and store it into ".huffman" file.

3. Decoding part read ".huffman"'s header, and construct huffman tree. Then read the other binary codes according to the huffman tree to decode the corresponding character.

4. Search part, decoding first, and then use KMP algorithm to find the matching string.
