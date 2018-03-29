#pragma once
#include <iostream>

class HufNode {
public:
	HufNode(unsigned char, unsigned int, HufNode*, HufNode*);
	~HufNode();
	unsigned char getByte();
	unsigned char getFreq();
	HufNode* getLeftChild();
	HufNode* getRightChild();
	bool isLeaf();
private:
	unsigned char byte;
	unsigned int freq;
	HufNode* leftChild;
	HufNode* rightChild;
};