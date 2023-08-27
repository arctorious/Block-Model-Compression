TIMER_FLAG =
COMPILER = g++
EXEC = main.out
ALGORITHM =

default:
	${COMPILER} -O3 -std=c++11 -pthread -Wall -o build/${EXEC} src/main.cpp src/compression.cpp src/stream_processor.cpp src/simple_compression.cpp src/runlength_encoding.cpp src/dp_compression.cpp


run:
	make
	./build/main.out < build/the_fast_one_376000_2x2x2.csv $(ALGORITHM) $(TIMER_FLAG)

run-simple:
	make run ALGORITHM=-s

run-runlength:
	make run ALGORITHM=-r

run-dp:
	make run ALGORITHM=-d

time:
	make run TIMER_FLAG=-t

time-simple:
	make run ALGORITHM=-s TIMER_FLAG=-t

time-runlength:
	make run ALGORITHM=-r TIMER_FLAG=-t

time-dp:
	make run ALGORITHM=-d TIMER_FLAG=-t

titan-linux:
	make COMPILER="x86_64-w64-mingw32-g++-posix -static" EXEC=BLOCK11.exe