#include <iostream>
#include <string>
#include "HufCompress.h"
#include "HufDecompress.h"
#include "HufConstants.h"

using namespace std;

int main(int argc, char* argv[]) {
	if (argc < 4) {
		cout << "Must include a mode [c/d], an input filepath and output filepath!" << endl;
		return 1;
	}

	string mode = argv[1];
	string inputFile = argv[2];
	string outputFile = argv[3];
	bool decompressMode = mode.compare("d") == 0;

	if (!decompressMode && !(mode.compare("c") == 0)) {
		cout << "Unknown mode entered.  Defaulting to compression" << endl;
	}

	string modeString = decompressMode ? "Decompression" : "Compression";
	cout << "Mode: " << modeString << endl;
	cout << "inputFile: " << inputFile << endl;
	cout << "outputFile: " << outputFile <<endl;

	int STATUS;

	if (decompressMode) {
		HufDecompress hufDecompress;
		STATUS = hufDecompress.decompressFile(inputFile, outputFile);
	} else {
		HufCompress hufCompress;
		STATUS = hufCompress.compressFile(inputFile, outputFile);
	}

	string successMessage = modeString + " Successful";
	string errorMessage = modeString + " Error!";
	string outputMessage = STATUS == HufConstants::SUCCESS ? successMessage : errorMessage;
	cout << outputMessage << "\n\n" ;

	return STATUS;
}