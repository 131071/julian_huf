#pragma once
#include <cstdio>
#include <string>
#include <stack>
#include "HufNode.h"
#include "HufConstants.h"

class HufDecompress {
public:
	HufDecompress();
	~HufDecompress();
	int decompressFile(std::string, std::string);
private:
	HufNode* readTreeHeader(FILE*);
};