#include "HufNode.h"

HufNode::HufNode(unsigned char byte, unsigned int freq, HufNode* leftChild, HufNode* rightChild) {
	this->byte = byte;
	this->freq = freq;
	this->leftChild = leftChild;
	this->rightChild = rightChild;
}

HufNode::~HufNode() {
	if (!this->isLeaf()) {
		delete this->getLeftChild();
		delete this->getRightChild();
	}
}

unsigned char HufNode::getByte() {
	return byte;
}

unsigned char HufNode::getFreq() {
	return freq;
}

HufNode* HufNode::getLeftChild() {
	return leftChild;
}

HufNode* HufNode::getRightChild() {
	return rightChild;
}

bool HufNode::isLeaf() {
	return ((leftChild == NULL) && (rightChild == NULL));
}
