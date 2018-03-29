#!/bin/bash
if [ -f "./test_output.txt" ]; then rm ./test_output.txt; fi
if [ -f "./test_input_decompressed.txt" ]; then rm ./test_input_decompressed.txt; fi
if [ -f "./sample_output.txt" ]; then rm ./sample_output.txt; fi
if [ -f "./sample_input_decompressed.txt" ]; then rm ./test_input_decompressed.txt; fi
if [ -f "./HUFCOMP" ]; then rm ./HUFCOMP; fi
if [ -f "./main.o" ]; then rm ./main.o; fi
if [ -f "./HufDecompress.o" ]; then rm ./HufDecompress.o; fi
if [ -f "./HufCompress.o" ]; then rm ./HufCompress.o; fi
if [ -f "./HufHeap.o" ]; then rm ./HufHeap.o; fi
if [ -f "./HufNode.o" ]; then rm ./HufNode.o; fi
make
./HUFCOMP c test_input.txt test_output.txt
./HUFCOMP d test_output.txt test_input_decompressed.txt
./HUFCOMP c sample_input.txt sample_output.txt
./HUFCOMP d sample_output.txt sample_input_decompressed.txt
