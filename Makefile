TIMER_FLAG =
COMPILER = g++
EXEC = main.out
ALGORITHM =
INPUT = tests/the_intro_one_32768_4x4x4.csv

.PHONY: build run run-simple run-runlength run-octree time time-simple time-runlength time-octree titan-linux titan-arch-linux runner-linux

build:
	${COMPILER} -O3 -std=c++11 -pthread -Wall -o build/${EXEC} src/main.cpp src/compression.cpp src/stream_processor.cpp src/simple_compression.cpp src/runlength_encoding.cpp src/octree_node.cpp src/octree_compression.cpp

run: build
	./build/main.out < ${INPUT} $(ALGORITHM) $(TIMER_FLAG)

run-simple:
	make run ALGORITHM=-s

run-runlength:
	make run ALGORITHM=-r

run-octree:
	make run ALGORITHM=-o

time:
	make run TIMER_FLAG=-t

time-simple:
	make run ALGORITHM=-s TIMER_FLAG=-t

time-runlength:
	make run ALGORITHM=-r TIMER_FLAG=-t

time-octree:
	make run ALGORITHM=-o TIMER_FLAG=-t

titan-linux:
	make COMPILER="x86_64-w64-mingw32-g++-posix -static" EXEC=BLOCK11.exe

titan-arch-linux:
	make COMPILER="x86_64-w64-mingw32-g++ -static" EXEC=BLOCK11.exe
	
runner-linux-intro:
	cd build; \
	wine python runner.py BLOCK11.exe the_intro_one_32768_4x4x4.csv -r -s

runner-linux-fast:
	cd build; \
	wine python runner.py BLOCK11.exe the_fast_one_376000_2x2x2.csv -r -s

runner-macos-intro:
	cd build; \
	wine64 python runner.py BLOCK11.exe the_intro_one_32768_4x4x4.csv -r -s

runner-macos-fast:
	cd build; \
	wine64 python runner.py BLOCK11.exe the_fast_one_376000_2x2x2.csv -r -s
