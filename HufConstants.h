#pragma once

class HufConstants {
public:
	static const int SUCCESS = 0;
	static const int ERROR = 1;
	static const int DEFAULT_BUFFER_LENGTH = 100;
	static const unsigned int ALPHA_THRESHOLD = 65;
	static const unsigned int SYSTEM_UNPRINTABLE_THRESHOLD = 32;
	static const unsigned int DEFAULT_COMPRESSION_BUFFER_LENGTH = 33;
	static const unsigned int DISTINCT_BYTES = 256;

	static const unsigned char DELETE_CHAR = 0x7F;
private:
};