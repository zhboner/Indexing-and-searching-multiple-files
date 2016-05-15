#define vectorLength 128 * 129 + 1
#define blockSize 2

#include <iostream>
#include <fstream>
#include <dirent.h>
#include <vector>
#include <algorithm>

using namespace std;

class pNode{
public:
    string pattern;
    string prefix;
    pNode *next;
public:
    pNode(string pat, string pre){
        pattern = pat;
        prefix = pre;
        next = NULL;
    }
};

class fNode{
public:
	string file;
	int match;
public:
	fNode(string f, int m){
		file = f;
		match = m;
	}
};

void readByLine(string, vector<fNode> *, struct dirent *, int);

vector<int> shiftTable;
vector<pNode *> suffixTable = vector<pNode *>(vectorLength, NULL);

int maxCommon = 300;
int fixArguments = 3;


void check(){
    int base = shiftTable[0];
    pNode *node;

    cout << "=========Test suffix table=========" << endl;
    for(int i = 0; i < vectorLength; ++i){
        node = suffixTable[i];
        if(suffixTable[i] != NULL) {
            while(node != NULL){
                cout << i << ": " << node->pattern << " - " << node->prefix;
                if(node->next != NULL){
                    cout << " <-- ";
                }
                node = node->next;
            }
            cout << endl;
        }
    }

    cout << "=========Test shift table=========" << endl;
    for(int j = 0; j < vectorLength; ++j){
        if(shiftTable[j] != base){
            cout << j << ": " << shiftTable[j] << endl;
        }
    }
}

bool compare(fNode x, fNode y){
	if(x.match != y.match){
		return x.match > y.match;
	}
	return x.file < y.file;
}
bool stringMatch(string x, string y){
//	transform(x.begin(), x.end(), x.begin(), ::tolower);
//	transform(y.begin(), y.end(), y.begin(), ::tolower);
	if(x == y) return true;
	return false;
}

int hash_any(string patternBlock){
	int x = patternBlock.at(0);
	int y = patternBlock.at(1);
	if(x >= 65 && x <= 90) x += 32;
	if(y >= 65 && y <= 90) y += 32;
	return 128 * x + y;
}

void buildTables(int argv, char *arg[]){
	int NOofPatterns = argv - fixArguments;
    string pattern[NOofPatterns];
    string block;
    int hashValue;
    int shiftValue;
    pNode *node;

    for(int i = 0; i < NOofPatterns; ++i){              // Set maxCommon value and get pattern array
        pattern[i] = arg[fixArguments + i];
        transform(pattern[i].begin(), pattern[i].end(), pattern[i].begin(), ::tolower);

        if(maxCommon > pattern[i].length()){
            maxCommon = (int) pattern[i].length();
        }
    }

	shiftTable = vector<int>(vectorLength, maxCommon - blockSize + 1);


    // Scan patterns to build two tables
    for(int j = 0; j < NOofPatterns; ++j){
        for(int m = 0; m < maxCommon - 1; m++){
            block = pattern[j].substr((unsigned int) m, blockSize);
            hashValue = hash_any(block);
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

int main(int argv, char *arg[]) {
    if(arg[3] == "-s") fixArguments = 5;
    buildTables(argv, arg);

	// Read directory
	string directory = arg[1];
	string path;
	DIR *dir;
	struct dirent *dirent;
//	string line;

	vector<fNode> okFile = vector<fNode>();

//	int matches = 0;                                  // The number of matches
//	unsigned long rIndex = 1;                         // Read index
//	unsigned long cIndex = 0;                         // Compare index (extract block from line)
//	string candidate;
//	int canHash = 0;
//	pNode *node;

	if((dir = opendir(arg[1])) != NULL){
		while((dirent = readdir(dir)) != NULL){
			if(string(dirent->d_name) == "." || string(dirent->d_name) == "..") continue;
			path = directory + "/" + dirent->d_name;

			// Read a file
			readByLine(path, &okFile, dirent, argv);
		}
	}

	// When finish reading, sort and output result
	sort(okFile.begin(), okFile.end(), compare);
	for(vector<fNode>::iterator ite = okFile.begin(); ite != okFile.end(); ite++){
//		cout << ite->file << ": " << ite->match << endl;
		cout << ite->file << endl;
	}

	return 0;
}

void readByLine(string path, vector<fNode> *okFile, struct dirent *dirent, int argv){
	ifstream file(path);
	string line;

	vector<string> patterns(0);                             // Store matched patterns for each file
	int matches = 0;                                  // The number of matches
	unsigned long rIndex = 1;                         // Read index
	unsigned long cIndex = 0;                         // Compare index (extract block from line)
	string candidate;
	int canHash = 0;
	pNode *node;

	while (getline(file, line)){
		transform(line.begin(), line.end(), line.begin(), ::tolower);

		rIndex = 1; cIndex = 0;

		while(rIndex < line.length()){                          // Read character one by one
			candidate = line.substr(rIndex - 1, 2);
			canHash = hash_any(candidate);

			if(shiftTable[canHash] != 0){
				// Go ahead if not the end of a pattern
				rIndex += shiftTable[canHash];
				if(rIndex >= line.length()){                // TODO: Considering change line and compare again
//					rIndex = 1; cIndex = 0;
					break;
				}
			}
			else{
				// At the end of a pattern
				if(rIndex + 1 < maxCommon){
					rIndex++;
					continue;
				}
				cIndex = rIndex + 1 - maxCommon;
				node = suffixTable[canHash];
				while(node != NULL){                        // Compare each pattern with identical suffix
					if(stringMatch(line.substr(cIndex, 2), node->prefix)){
						// If prefix matched. then compare all string
						if(cIndex + node->pattern.length() > line.length()){
							rIndex += node->pattern.length();
							break;
						}
						if(stringMatch(line.substr(cIndex, node->pattern.length()), node->pattern)){       // Match
							matches++;
							patterns.push_back(node->pattern);
							sort(patterns.begin(), patterns.end());
							vector<string>::iterator iter = unique(patterns.begin(), patterns.end());
							patterns.erase(iter, patterns.end());
						}
					}
					node = node->next;
					cIndex = rIndex + 1 - maxCommon;
				}
				rIndex++;
			}
		}
	}
	file.close();

	if(patterns.size() == argv - fixArguments){                                 // If all patterns are matched
		okFile->push_back(fNode(dirent->d_name, matches));
	}

//	patterns.empty();


}
