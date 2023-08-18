TIMER_FLAG =
COMPILER = g++
EXEC = main.out
FLAG = 

default:
	${COMPILER} -std=c++11 -Wall -o build/${EXEC} ${FLAG} src/main.cpp src/compression.cpp src/stream_processor.cpp src/simple_compression.cpp src/runlength_encoding.cpp

run:
	make
	./build/main.out < build/the_intro_one_32768_4x4x4.csv $(TIMER_FLAG)

time:
	make run TIMER_FLAG=-t

titan-linux:
	make COMPILER="x86_64-w64-mingw32-g++ -static" EXEC=BLOCK11.exe