#include <cstring>
#include <set>
#include "main.h"

vector<int> shiftTable;
vector<pNode *> suffixTable = vector<pNode *>(vectorLength, NULL);
vector<string> pattern;

int maxCommon = 300;
int fixArguments = 3;
unsigned long NOofPatterns;

void buildTables(int argv, char *arg[]){
	NOofPatterns = (unsigned long)argv - fixArguments;
	pattern = vector<string>(NOofPatterns);
    string block;
    int hashValue;
    int shiftValue;
    pNode *node;
//	int hashTable[4];
	set<int> hashTable;

	char asc1, asc2;

    for(int i = 0; i < NOofPatterns; ++i){              // Set maxCommon value and get pattern array
        pattern[i] = arg[fixArguments + i];
        transform(pattern[i].begin(), pattern[i].end(), pattern[i].begin(), ::tolower);

        if(maxCommon > pattern[i].length()){
            maxCommon = (int) pattern[i].length();
        }
    }
	// Initialize shift table
	shiftTable = vector<int>(vectorLength, maxCommon);
	for(int j = 0; j < NOofPatterns; ++j){
		asc2 = pattern[j].at(0);
		for(int k = 0; k < 256; ++k){
			shiftTable[hash_block(k, (int) asc2)] = maxCommon - blockSize + 1;
			shiftTable[hash_block(k, toupper(asc2))] = maxCommon - blockSize + 1;

		}
	}

    // Scan patterns to build two tables
    for(int j = 0; j < NOofPatterns; ++j){
        for(int m = 0; m < maxCommon - 1; m++){
            block = pattern[j].substr((unsigned int) m, blockSize);

	        asc1 = block.at(0), asc2 = block.at(1);
	        hashTable.clear();
	        hashTable.insert(hash_block(tolower(asc1), tolower(asc2)));
	        hashTable.insert(hash_block(toupper(asc1), toupper(asc2)));
	        hashTable.insert(hash_block(tolower(asc1), toupper(asc2)));
	        hashTable.insert(hash_block(toupper(asc1), tolower(asc2)));

	        for(set<int>::iterator i = hashTable.begin(); i != hashTable.end(); ++i){
		        hashValue = *i;
		        shiftValue = maxCommon - 2 - m;
		        if(shiftValue < shiftTable[hashValue]){          // Build shift table
			        shiftTable[hashValue] = shiftValue;
		        }
		        if(shiftValue == 0){                            // Build suffix table
			        node = new pNode(pattern[j], pattern[j].substr(0, blockSize));
			        if(suffixTable[hashValue] != NULL){
				        pNode *tmp = suffixTable[hashValue];
				        node->next = tmp;
				        suffixTable[hashValue] = node;
			        }
			        else{
				        suffixTable[hashValue] = node;
			        }
		        }
	        }
        }
    }
}

int main(int argv, char *arg[]) {
	if(!strcmp(arg[3], "-s")) fixArguments = 5;
    buildTables(argv, arg);

	// Read directory
	string directory = arg[1];
	string path;
	DIR *dir;
	struct dirent *dirent;
	vector<fNode> okFile = vector<fNode>();

	if((dir = opendir(arg[1])) != NULL){
		while((dirent = readdir(dir)) != NULL){
			if(string(dirent->d_name) == "." || string(dirent->d_name) == "..") continue;
			path = directory + "/" + dirent->d_name;

			// Read a file
			readByLine(path, &okFile, dirent, argv);
//			readWholeFile(path, &okFile, dirent);
		}
	}

	// When finish reading, sort and output result
	sort(okFile.begin(), okFile.end(), compare);
	for(vector<fNode>::iterator ite = okFile.begin(); ite != okFile.end(); ite++){
//		cout << ite->file << ": " << ite->match << endl;
		cout << ite->file << endl;
	}

	clean();
	return 0;
}

void readByLine(string path, vector<fNode> *okFile, struct dirent *dirent, int argv){
	ifstream file(path);
	string line;
	int matches = 0;                                  // The number of matches
	unsigned long rIndex = 1;                         // Read index
	unsigned long cIndex = 0;                         // Compare index (extract block from line)
	int canHash = 0;
	pNode *node;
	vector<bool> matchedPattern(NOofPatterns, false);

	char asc1, asc2;
	while (getline(file, line)){
		rIndex = (unsigned long) maxCommon - blockSize + 1; cIndex = 0;

		while(rIndex < line.length()){                          // Read character one by one
			asc2 = line.at(rIndex);
			if((asc2 != 32 && asc2 < 65) || asc2 > 122 || (asc2 > 90 && asc2 < 97)){
				rIndex += maxCommon;
				continue;
			}
			asc1 = line.at(rIndex - 1);
			if((asc1 != 32 && asc1 < 65) || asc1 > 122 || (asc1 > 90 && asc1 < 97)){
				rIndex += maxCommon - blockSize + 1;
				continue;
			}
			canHash = hash_any((int) asc1, (int) asc2);

			if(shiftTable[canHash] != 0){
				// Go ahead if not the end of a pattern
				rIndex += shiftTable[canHash];
				if(rIndex >= line.length()){                // TODO: Considering change line and compare again
					break;
				}
			}
			else{
				// At the end of a pattern
				cIndex = rIndex + 1 - maxCommon;
				node = suffixTable[canHash];
				while(node != NULL){                        // Compare each pattern with identical suffix
					if(stringMatch(cIndex, node->prefix.length(), &line, node->prefix)){
						// If prefix matched. then compare all string
						if(cIndex + node->pattern.length() > line.length()){
							rIndex += node->pattern.length();
							break;
						}
						if(stringMatch(cIndex, node->pattern.length(), &line, node->pattern)){       // Match
							matches++;
							for(int i = 0; i < NOofPatterns; i++){
								if(pattern[i] == node->pattern){
									matchedPattern[i] = true;
									break;
								}
							}
						}
					}
					node = node->next;
					cIndex = rIndex + 1 - maxCommon;
				}
				rIndex += 1;
			}
		}
	}
	file.close();

	if(checkAllPatternMatched(&matchedPattern)){                                 // If all patterns are matched
		okFile->push_back(fNode(dirent->d_name, matches));
	}

//	patterns.empty();


}

void readWholeFile(string path, vector<fNode> *okFile, struct dirent *dirent){
	int matches = 0;                                                                                // The number of matches
	unsigned long rIndex = (unsigned long) maxCommon - blockSize + 1;                               // Read index
	unsigned long cIndex = 0;                                                // Compare index (extract block from line)
//	string candidate;
	int canHash = 0;
	pNode *node;
	vector<bool> matchedPattern(NOofPatterns, false);

	char asc1, asc2;
	string line = readFile(path);

	while(rIndex < line.length()){                          // Read character one by one
		asc2 = line.at(rIndex);
		if((asc2 != 32 && asc2 < 65) || asc2 > 122 || (asc2 > 90 && asc2 < 97)){
			rIndex += maxCommon;
			continue;
		}
		asc1 = line.at(rIndex - 1);
		if((asc1 != 32 && asc1 < 65) || asc1 > 122 || (asc1 > 90 && asc1 < 97)){
			rIndex += maxCommon - blockSize + 1;
			continue;
		}
		canHash = hash_any((int) asc1, (int) asc2);

		if(shiftTable[canHash] != 0){
			// Go ahead if not the end of a pattern
			rIndex += shiftTable[canHash];
			if(rIndex >= line.length()){
				break;
			}
		}
		else{                   // At the end of a pattern
			cIndex = rIndex + 1 - maxCommon;
			node = suffixTable[canHash];
			while(node != NULL){                        // Compare each pattern with identical suffix
				if(stringMatch(cIndex, node->prefix.length(), &line, node->prefix)){
					// If prefix matched. then compare all string

					// If the length of string is shorter than pattern, stop
					if(cIndex + node->pattern.length() > line.length()){
						rIndex += node->pattern.length();
						break;
					}
					if(stringMatch(cIndex, node->pattern.length(), &line, node->pattern)){       // Match
						matches++;
						for(int i = 0; i < NOofPatterns; i++){
							if(pattern[i] == node->pattern){
								matchedPattern[i] = true;
								break;
							}
						}
					}
				}
				node = node->next;
				cIndex = rIndex + 1 - maxCommon;
			}
			rIndex += 1;
		}
	}
	if(checkAllPatternMatched(&matchedPattern)){                                 // If all patterns are matched
		okFile->push_back(fNode(dirent->d_name, matches));
	}
}

string readFile(string path){
	ifstream f(path);
	stringstream strStream;
	strStream << f.rdbuf();
	return strStream.str();
}
bool compare(fNode x, fNode y){
	if(x.match != y.match){
		return x.match > y.match;
	}
	return x.file < y.file;
}


bool stringMatch(unsigned long cIndex, unsigned int length, string *source, string target){
	char x;
	char y;
	for(unsigned int i = 0; i < length; ++i){
		x = source->at(cIndex + i);
		y = target.at(i);
		if(tolower(x) != (int) y) return false;
	}
	return true;
}

int hash_any(int x, int y){
//	int x = patternBlock.at(0);
//	int y = patternBlock.at(1);
//	if(x >= 65 && x <= 90) x += 32;
//	if(y >= 65 && y <= 90) y += 32;
	return 128 * x + y;
}

bool checkAllPatternMatched(vector<bool> *p){
	for(int i = 0; i < NOofPatterns; i++){
		if(p->at(i) != true) return false;
	}
	return true;
}

int hash_block(int x, int y){
	return 128 * x + y;
}

bool checkString(string x, string y){
	transform(x.begin(), x.end(), x.begin(), ::tolower);
	transform(y.begin(), y.end(), y.begin(), ::tolower);
	return x == y;
}

void clean(){
	pNode *node, *tmp;
	for(unsigned long i = 0; i < vectorLength; ++i){
		if(suffixTable[i] != NULL){
			node = suffixTable[i];
			while(node != NULL){
				tmp = node;
				node =node->next;
				delete tmp;
			}
		}
	}
}
