//
// Created by zhboner on 5/15/16.
//

#ifndef INC_3_MAIN_H
#define INC_3_MAIN_H

#define vectorLength 128 * 129 + 1
#define blockSize 2

#include <iostream>
#include <fstream>
#include <dirent.h>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>

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
bool compare(fNode, fNode);
bool stringMatch(string x, string y);
int hash_any(string patternBlock);
void buildTables(int argv, char *arg[]);
void readWholeFile(string path, vector<fNode> *okFile, struct dirent *dirent, int argv);
string readFile(string path);
#endif //INC_3_MAIN_H
