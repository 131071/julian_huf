#pragma once
#include <iostream>
#include <cstdio>
#include <string>
#include <string.h>
#include <vector>
#include "HufHeap.h"
#include "HufNode.h"
#include "HufConstants.h"

class HufCompress {
public:
	HufCompress();
	~HufCompress();
	int compressFile(std::string, std::string);	
private:
	int tabulateFile(std::string, unsigned int[]);
	HufNode* buildTree(unsigned int[]);
	HufHeap buildHeap(unsigned int[]);
	HufNode* buildTree(HufHeap);
	int encodeTreeHeader(unsigned char**, HufNode*);
	void encodeTree(std::vector<unsigned char>*, HufNode*);
	void encodePaths(char*[], HufNode*);
	void encodePaths(char*[], HufNode*, std::string);
	int processFiles(int, unsigned char*, int, char*[], std::string, std::string);
	unsigned char chompBuffer(char*);
	int getMaxPathForNode(HufNode*);
	void freePaths(char*[]);
};