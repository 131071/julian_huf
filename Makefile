HUFCOMP: main.o HufHeap.o HufNode.o HufCompress.o HufDecompress.o
	g++ main.o HufHeap.o HufNode.o HufCompress.o HufDecompress.o -o HUFCOMP

main.o: main.cc HufCompress.h HufDecompress.h HufConstants.h
	g++ -c -g main.cc

HufDecompress.o: HufDecompress.cc HufNode.h HufConstants.h
	g++ -c -g HufDecompress.cc

HufCompress.o: HufCompress.cc HufHeap.h HufNode.h HufConstants.h
	g++ -c -g HufCompress.cc

HufHeap.o: HufHeap.cc HufNode.h
	g++ -c -g HufHeap.cc

HufNode.o: HufNode.cc
	g++ -c -g HufNode.cc
