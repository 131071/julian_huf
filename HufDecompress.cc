#include "HufDecompress.h"


HufDecompress::HufDecompress() {
}

HufDecompress::~HufDecompress() {
}

/**
 * Reads header and builds huffman tree from it, then decompresses remaining bytes using
 * built tree.
 * The last byte of the input file denotes how many bits are to be used from the previous
 * byte of compressed data.
 * The delimiter between tree header and compressed data is "/\"
 */
int HufDecompress::decompressFile(std::string inputFilePath, std::string outputFilePath) {
	FILE *fpr, *fpw;

	fpr = fopen(inputFilePath.c_str(), "rb");
	fpw = fopen (outputFilePath.c_str(), "wb");

	if ((fpr == NULL) || (fpw == NULL)) {
		return HufConstants::ERROR;
	}

	HufNode *root = readTreeHeader(fpr);
	if (root == NULL) {
		return HufConstants::ERROR;
	}

	HufNode *pointer = root;
	unsigned char buffer[HufConstants::DEFAULT_BUFFER_LENGTH];
	unsigned char byte, decompressedByte;
	int bytesRead;
	int bitsUsed;
	bool lastRound = false;

	while (!feof(fpr)) {
		bytesRead = fread(buffer, sizeof(unsigned char), sizeof(buffer), fpr);
		lastRound = feof(fpr);
		bytesRead = lastRound ? bytesRead - 1 : bytesRead;
		for (int i = 0; i < bytesRead; i++) {
			bitsUsed = lastRound && (i == (bytesRead - 1)) ? buffer[bytesRead] : 8;
			byte = buffer[i];
			for (int j = 0; j < bitsUsed; j++) {
				if (byte & (0x80 >> j)) {
					pointer = pointer->getRightChild();
				} else {
					pointer = pointer->getLeftChild();
				}
				if (pointer->isLeaf()) {
					decompressedByte = pointer->getByte();
					fwrite(&decompressedByte, sizeof(unsigned char), 1, fpw);
					pointer = root;
				}
			}
		}
	}

	fclose(fpr);
	fclose(fpw);
	delete root;
	return HufConstants::SUCCESS;
}

/**
 * Delimiter for end of header is "/\".
 * Parents are encoded as '*'.
 * System chars, unprintable chars, and '*' and '/' are escaped.
 * System chars are escaped to alphabetic chars.
 * All other bytes are encoded directly.
 */
HufNode* HufDecompress::readTreeHeader(FILE *fp) {
	HufNode *node, *leftNode, *rightNode;
	std::stack<HufNode*> nodes;
	unsigned char byte;
	bool treeRead = false;

	while(!treeRead) {
		if (feof(fp)) {
			return NULL;
		}
		fread(&byte, sizeof(unsigned char), 1, fp);

		if (byte == '/') {
			if (feof(fp)) {
				return NULL;
			}
			fread(&byte, sizeof(unsigned char), 1, fp);

			if (byte == '\\') {
				treeRead = true;
				break;
			} else {
				if (byte == '0') {
					byte = HufConstants::DELETE_CHAR;
				} else if (byte >= HufConstants::ALPHA_THRESHOLD) {
					byte -= HufConstants::ALPHA_THRESHOLD;
				}

				node = new HufNode(byte, 0, NULL, NULL);
				nodes.push(node);
			}
		} else if (byte == '*') {
			rightNode = nodes.top();
			nodes.pop();
			leftNode = nodes.top();
			nodes.pop();
			node = new HufNode(0, 0, leftNode, rightNode);
			nodes.push(node);
		} else {
			node = new HufNode(byte, 0, NULL, NULL);
			nodes.push(node);
		}
	}

	if (nodes.size() != 1) {
		std::cout << "\nStack size not equal to 1, tree building issue." << std::endl;
		return NULL;
	}

	return nodes.top();
}
