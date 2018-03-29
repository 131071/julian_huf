#include "HufHeap.h"


HufHeap::HufHeap() {
	this->heapSize = 0;
}

HufHeap::~HufHeap() {
}

bool HufHeap::isEmpty() {
	return (heapSize < 1);
}

void HufHeap::insert(HufNode* nNode) {
	heapSize++;
	heap[heapSize] = nNode;
	siftUp(heapSize);
}

HufNode* HufHeap::extract() {
	if (isEmpty()) {
		return NULL;
	}
	HufNode* t = heap[1];
	heap[1] = heap[heapSize];
	heapSize--;
	siftDown(1);
	return t;
}

void HufHeap::siftDown(int node) {
	int min = node * 2;
	if ((node >= 1) && (min <= heapSize)) {
		if ( ((min + 1) <= heapSize) && (heap[min + 1]->getFreq() < heap[min]->getFreq()) ) {
			min++;
		}

		if (heap[min]->getFreq() < heap[node]->getFreq()) {
			swap(node, min);
			siftDown(min);
		}
	}
}

void HufHeap::siftUp(int node) {
	if (node > 1 && node <= heapSize) {
		int parent = node / 2;
		if ( heap[node]->getFreq() < heap[parent]->getFreq() ) {
			swap(node, parent);
			siftUp(parent);
		}
	}
}

void HufHeap::swap(int a, int b) {
	HufNode* t = heap[a];
	heap[a] = heap[b];
	heap[b] = t;
}
