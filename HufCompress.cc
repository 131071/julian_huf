#include "HufCompress.h"

HufCompress::HufCompress() {
}

HufCompress::~HufCompress() {
}

/**
 * Orchestrator function for file compression
 */
int HufCompress::compressFile(std::string inputFilePath, std::string outputFilePath) {
	unsigned int frequencyTable[HufConstants::DISTINCT_BYTES];
	if (tabulateFile(inputFilePath, frequencyTable) == HufConstants::ERROR) {
		std::cout << "Problem opening input file" << std::endl;
		return HufConstants::ERROR;
	}

	HufNode* root = buildTree(frequencyTable);

	unsigned char* encodedTree;
	int encodedTreeLength = encodeTreeHeader(&encodedTree, root);

	char* pathTable[HufConstants::DISTINCT_BYTES];
	for (int i = 0; i < HufConstants::DISTINCT_BYTES; i++) {
		pathTable[i] = NULL;
	}	
	encodePaths(pathTable, root);
	
	int maxPathLength = getMaxPathForNode(root);
	delete root;

	int status = processFiles(maxPathLength, encodedTree, encodedTreeLength, pathTable, inputFilePath, outputFilePath);

	delete encodedTree;
	freePaths(pathTable);
	return status;
}

/**
 * Counts how many times each byte occurs in the file for the given file path
 */
int HufCompress::tabulateFile(std::string inputFilePath, unsigned int frequencyTable[HufConstants::DISTINCT_BYTES]) {
	FILE *fp;
	unsigned char buffer[HufConstants::DEFAULT_BUFFER_LENGTH];
	int bytesRead;

	for (int i = 0; i < HufConstants::DISTINCT_BYTES; i++) {
		frequencyTable[i] = 0;
	}

	fp = fopen(inputFilePath.c_str(), "rb");
	if (fp == NULL) {
		return HufConstants::ERROR;
	}
	while (!feof(fp)) {
		bytesRead = fread(buffer, sizeof(unsigned char), sizeof(buffer), fp);
		for (int i = 0; i < bytesRead; i++) {
			frequencyTable[(unsigned int) buffer[i]]++;
		}
	}
	fclose(fp);
	return HufConstants::SUCCESS;
}

/**
 * Builds a huffman tree such that the most frequent bytes have the shortest path
 * from the root and least-frequent bytes have the longest path from the root
 */
HufNode* HufCompress::buildTree(unsigned int frequencyTable[HufConstants::DISTINCT_BYTES]) {
	HufHeap heap = buildHeap(frequencyTable);
	return buildTree(heap);
}

HufHeap HufCompress::buildHeap(unsigned int frequencyTable[HufConstants::DISTINCT_BYTES]) {
	HufHeap heap;
	HufNode *node;

	for (int i = 0; i < HufConstants::DISTINCT_BYTES; i++) {
		if (frequencyTable[i] > 0) {
			node = new HufNode((unsigned char) i, frequencyTable[i], NULL, NULL);
			heap.insert(node);
		}
	}
	return heap;
}

HufNode* HufCompress::buildTree(HufHeap heap) {
	HufNode *node, *leftNode, *parentNode;

	while (!heap.isEmpty()) {
		node = heap.extract();
		if (heap.isEmpty()) {
			break;
		}
		leftNode = heap.extract();
		parentNode = new HufNode(0, leftNode->getFreq() + node->getFreq(), leftNode, node);
		heap.insert(parentNode);
	}

	return node;
}

/**
 * Builds a byte array representing the tree in post-traversal notation.
 * The encoded tree is then padded with the header delimiter /\ marking
 * separation between header and compressed data.  The amount of bytes 
 * copied to the encodedTreeHeader is returned
 */
int HufCompress::encodeTreeHeader(unsigned char** encodedTreeHeader, HufNode* node) {
	std::vector<unsigned char> encodedTreeBytes;
	encodeTree(&encodedTreeBytes, node);
	encodedTreeBytes.push_back('/');
	encodedTreeBytes.push_back('\\');

	int encodedTreeLength = encodedTreeBytes.size();
	*encodedTreeHeader = new unsigned char[encodedTreeLength];
	int i = 0;
	for (std::vector<unsigned char>::iterator it = encodedTreeBytes.begin(); 
			it != encodedTreeBytes.end(); 
			++it) {
		unsigned char byte = *it;
		(*(encodedTreeHeader))[i] = byte;
		i++;
	}
	return encodedTreeLength;
}

/**
 * in encoding, '/' is used for an escape character
 * asterisks signify parent nodes in post-traversal notation.
 * both asterisks and backslashes are escaped with their literal,
 * while unprintable and system characters are escaped by incrementing
 * their value to an alpha-numeric after an escape char.
 */
void HufCompress::encodeTree(std::vector<unsigned char>* encodedTreeBytes, HufNode* node) {
	if (node == NULL) {
		return;
	}
	if (node->isLeaf()) {
		unsigned char byte = node->getByte();
		if (byte == '*' || byte == '/') {
			encodedTreeBytes->push_back('/');
		} else if (byte == HufConstants::DELETE_CHAR) {
			encodedTreeBytes->push_back('/');
			byte = '0';
		} else if (byte <= HufConstants::SYSTEM_UNPRINTABLE_THRESHOLD) {
			encodedTreeBytes->push_back('/');
			byte += HufConstants::ALPHA_THRESHOLD;
		}
		encodedTreeBytes->push_back(byte);
	} else {
		encodeTree(encodedTreeBytes, node->getLeftChild());
		encodeTree(encodedTreeBytes, node->getRightChild());
		encodedTreeBytes->push_back('*');
	}
}

void HufCompress::encodePaths(char* pathTable[HufConstants::DISTINCT_BYTES], HufNode* node) {
	freePaths(pathTable);
	encodePaths(pathTable, node, "");
}

/**
 * All encoded paths are C-string representations of their binary bitstrings 
 * where '0' denotes a left child traversal and '1' denotes a right traversal
 */
void HufCompress::encodePaths(char* pathTable[HufConstants::DISTINCT_BYTES], HufNode* node, std::string originalPath) {
	if (node->isLeaf()) {
		int length = originalPath.length();
		unsigned int index = (unsigned int) node->getByte();
		pathTable[index] = new char[length + 1];
		char* indexedPath = pathTable[index];
		strcpy(indexedPath, originalPath.c_str());
	} else {
		encodePaths(pathTable, node->getLeftChild(), originalPath + "0");
		encodePaths(pathTable, node->getRightChild(), originalPath + "1");
	}
}

/**
 * The final compressed file is started with the tree-encoding header (including pad).
 * The remaining contents of output file is compressed data.
 * The final byte of the output file represents the significant bits of the previous byte 
 * of compressed data.
 */
int HufCompress::processFiles(int maxPathLength, unsigned char* treeHeader, int headerLength, 
								char* pathTable[HufConstants::DISTINCT_BYTES], std::string inputFilePath, 
								std::string outputFilePath) {
	FILE *fpr, *fpw;
	unsigned char in_buffer[HufConstants::DEFAULT_BUFFER_LENGTH];
	char* compression_buffer; 
	int bytesRead;
	unsigned char compressedByte;

	fpr = fopen(inputFilePath.c_str(), "rb");
	fpw = fopen (outputFilePath.c_str(), "wb");

	if ((fpr == NULL) || (fpw == NULL)) {
		return HufConstants::ERROR;
	}

	fwrite(treeHeader, sizeof(unsigned char), headerLength, fpw);

	int compression_buffer_length = (2 * maxPathLength) + 1;
	compression_buffer_length = HufConstants::DEFAULT_COMPRESSION_BUFFER_LENGTH > compression_buffer_length ? 
		HufConstants::DEFAULT_COMPRESSION_BUFFER_LENGTH : compression_buffer_length;
	compression_buffer = new char[compression_buffer_length];
	compression_buffer[0] = 0x00;

	while (!feof(fpr)) {
		bytesRead = fread(in_buffer, sizeof(unsigned char), sizeof(in_buffer), fpr);
		for (int i = 0; i < bytesRead; i++) {
			unsigned char byte = in_buffer[i];
			char* path = pathTable[(unsigned int) byte];
			strcat(compression_buffer, path);
			int length = strlen(compression_buffer);
			while (length >= 8) {
				compressedByte = chompBuffer(compression_buffer);
				fwrite(&compressedByte, sizeof(unsigned char), 1, fpw);
				length = strlen(compression_buffer);
			}
		}
	}

	int usedBits = strlen(compression_buffer);
	usedBits = usedBits == 0 ? 8 : usedBits;
	for (int i = 0; i < (8 - usedBits); i++) {
		strcat(compression_buffer, "0");
	}

	if (usedBits < 8) {
		compressedByte = chompBuffer(compression_buffer);
		fwrite(&compressedByte, sizeof(unsigned char), 1, fpw);
	}
	unsigned char lastByte = (unsigned char) usedBits;
	fwrite(&lastByte, sizeof(unsigned char), 1, fpw);

	delete compression_buffer;
	fclose(fpr);
	fclose(fpw);
	return HufConstants::SUCCESS;
}

/**
 * Compressed the symbolic representation of a byte in a C-String into a 
 * proper byte, and removes the 8 characters representing the byte from the 
 * beginning of the buffer.
 */
unsigned char HufCompress::chompBuffer(char* buffer) {
	unsigned char byte = 0x00;

	if (strlen(buffer) < 8) {
		return byte;
	}

	char *t, *b;
	t = buffer;
	b = buffer;

	for (int i = 0; i < 8; i++) {
		if (*b == '1') {
			byte ^= (0x80 >> i);
		}
		b++;
	}

	while (*b != 0) {
		*t++ = *b++;
	}
	*t = 0;

	return byte;
}

/**
 * Determines the maximum path from the root to a leaf node,
 * to determine an appropriate size of the compression buffer.
 */
int HufCompress::getMaxPathForNode(HufNode *node) {
	if (node == NULL) {
		return 0;
	}
	if (node->isLeaf()) {
		return 1;
	} else {
		int leftMax = getMaxPathForNode(node->getLeftChild());
		int rightMax = getMaxPathForNode(node->getRightChild());
		int  max = leftMax > rightMax ? leftMax : rightMax;
		return max + 1;
	}
}

void HufCompress::freePaths(char* pathTable[HufConstants::DISTINCT_BYTES]) {
	for (int i = 0; i < HufConstants::DISTINCT_BYTES; i++) {
		char* path = pathTable[i];
		if (path) delete path;
		pathTable[i] = NULL;
	}
}

