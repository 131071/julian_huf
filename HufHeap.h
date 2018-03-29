#pragma once
#include <iostream>
#include "HufNode.h"

class HufHeap {
public:
	HufHeap();
	~HufHeap();
	bool isEmpty();
	void insert(HufNode*);
	HufNode* extract();
private:
	void siftDown(int);
	void siftUp(int);
	void swap(int, int);

	HufNode* heap[257];
	int heapSize;
};