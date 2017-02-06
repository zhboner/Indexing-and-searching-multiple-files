1. Wu Manber algorithm
Using Wu Manber algorithm to perform pattern search over files in the codes. By this algorithm, every two characters are regarded as a block. And build the 
pattern list by the length of the shortest pattern among patterns (longest common length). When searching over the files, program always compare the last 
block in max common length. If it doesn't match, then skiping the text according to the length in shift table (this is the first array the program take use 
of). If a block is matched, take next step that find the prefix of this pattern in the suffix table (the second array) and check if their prefix can be 
matched successfully. If matched successfully again, following the linked list (the third list used in the program) that containing the whole pattern and 
match it character by character. This program builds 3 data structures before searching.


2. Two vector and one linked list
The first vector is shift table. When building this table, program only care about text within the max common length. Shift table is a list that containing 
all blocks formed by all ASCII characters. Its length is 128 * 128 + 128 + 1. Each blocks corresponds an entry in this vector by hash_any() function. This 
vector is initialized by max common length and in each pattern if the distance from a block to the tail of the block is smaller than its entry value, 
program will update the corresponding vector entry to the smaller one.

The second vector is suffix table. Its length is the same as former. It contains each blocks and only the last block within max common length will have an 
entry to the pattern node (pNode).

The linked list is based on suffix table. If two or more patterns have the same last block, their pattern node instance will be linked. Therefore, there 
may be more than one linked list in terms of various patterns.


3. Two classes
pattern node (pNode) has three attributes. pNode.pattern is a string that save the specified pattern with the suffix corresponding to the index. 
pNode.prefix is the prefix of this pattern, recording this string just for convenient matching when searching over documents. pNode.next is a pointer to 
the next instace with the same suffix. If it is the last one, this pointer is NULL.

file node (fNode) is used when sorting the file names by specified rules. It has two attributes. fNode.file is a string of the file name and fNode.match is 
the amount of matches in this file.


4. Functions
buildTables(). This function is called before searching, which is used to build required vectors and linked lists based on input patterns.

readByLine(). This function is called for every file.This is the main function for search over text. It read file line by line and picks blocks from text. 
Then call hash_any() to get the entry of shift table. If the corresponding is 0, which means this is a tail block, then it will get the corresponding pNode
instance by suffix table and compare patterns in the linked list. If the value in shift table is not 0, then skip value and repeat the process.

compare(). Called by sort() when sorting files. 

stringMatch(). Called to compare patterns and source text. If matched successfully, return true. 

hash_any(). Change a block to a integer.

checkAllPatternMatched(). Called after searching a file to check if all patterns shown in the file. Return false if missing one or more patterns.

clean(). Release memory at the end of program.