TIMER_FLAG = 
COMPILER = g++
EXEC = main.out
ALGORITHM = -d
INPUT = 

# tar -xf gcc-13.2.0.tar.gz
# cd gcc-13.2.0
# ./contrib/download_prerequisites
# cd ..
# mkdir gcc-build
# cd gcc-build
# ../gcc-13.2.0/configure --prefix=/opt/gcc-13.2.0 --enable-languages=c,c++ --enable-libstdcxx-backtrace=yes --disable-option-checking
# make -j$(nproc) 

.PHONY: build run run-simple-intro run-simple-fast run-simple-comb run-runlength-intro run-runlength-fast run-runlength-comb run-octree-intro run-octree-fast run-octree-comb run-decomp3D-intro run-runlength3D-intro run-decomp3D-fast titan-linux titan-arch-linux runner-linux

build:
	${COMPILER} -O3 -std=c++11 -pthread -Wall -o build/${EXEC} src/main.cpp src/compression.cpp src/stream_processor.cpp src/simple_compression.cpp src/runlength_encoding.cpp src/octree_node.cpp src/octree_compression.cpp src/dp_compression.cpp src/Decomp3D.cpp src/runlength_encoding_3D.cpp

run: build
	./build/main.out < ${INPUT} $(ALGORITHM) $(TIMER_FLAG)

# RUN SIMPLE
run-simple-intro:
	make run INPUT=build/the_intro_one_32768_4x4x4.csv ALGORITHM=-s TIMER_FLAG=-t

run-simple-fast:
	make run INPUT=build/the_fast_one_376000_2x2x2.csv ALGORITHM=-s TIMER_FLAG=-t

run-simple-comb:
	make run INPUT=build/the_combinatorial_one_42000000_14x10x12.csv ALGORITHM=-s TIMER_FLAG=-t

# RUN RUNLENGTH
run-runlength-intro:
	make run INPUT=build/the_intro_one_32768_4x4x4.csv ALGORITHM=-r TIMER_FLAG=-t

run-runlength-fast:
	make run INPUT=build/the_fast_one_376000_2x2x2.csv ALGORITHM=-r TIMER_FLAG=-t

run-runlength-comb:
	make run INPUT=build/the_combinatorial_one_42000000_14x10x12.csv ALGORITHM=-r TIMER_FLAG=-t

# RUN OCTREE
run-octree-intro:
	make run INPUT=build/the_intro_one_32768_4x4x4.csv ALGORITHM=-o TIMER_FLAG=-t

run-octree-fast:
	make run INPUT=build/the_fast_one_376000_2x2x2.csv ALGORITHM=-o TIMER_FLAG=-t

run-octree-comb:
	make run INPUT=build/the_combinatorial_one_42000000_14x10x12.csv ALGORITHM=-o TIMER_FLAG=-t

#RUN RUNLENGTH 3D
run-runlength3D-intro:
	make run INPUT=build/the_intro_one_32768_4x4x4.csv ALGORITHM=-R TIMER_FLAG=-t

# RUN DECOMP 3D
run-decomp3D-intro:
	make run INPUT=build/the_intro_one_32768_4x4x4.csv ALGORITHM=-3 TIMER_FLAG=-t

run-decomp3D-fast:
	make run INPUT=build/the_fast_one_376000_2x2x2.csv ALGORITHM=-3 TIMER_FLAG=-t

run-decomp3D-comb:
	make run INPUT=build/the_combinatorial_one_42000000_14x10x12.csv ALGORITHM=-3 TIMER_FLAG=-t

# RUN DP
run-dp-intro:
	make run INPUT=build/the_intro_one_32768_4x4x4.csv ALGORITHM=-d TIMER_FLAG=-t

run-dp-fast:
	make run INPUT=build/the_fast_one_376000_2x2x2.csv ALGORITHM=-d TIMER_FLAG=-t

run-dp-comb:
	make run INPUT=build/the_combinatorial_one_42000000_14x10x12.csv ALGORITHM=-d TIMER_FLAG=-t

# TITAN BUILDS
titan-linux:
	make COMPILER="x86_64-w64-mingw32-g++-posix -static" EXEC=BLOCK11.exe

titan-macos:
	make COMPILER="x86_64-w64-mingw32-g++ -static" EXEC=BLOCK11.exe

titan-arch-linux:
	make COMPILER="x86_64-w64-mingw32-g++ -static" EXEC=BLOCK11.exe
	
# LOCAL TITAN RUNS
runner-linux-intro:
	cd build; \
	wine python runner.py BLOCK11.exe the_intro_one_32768_4x4x4.csv -r -s

runner-linux-fast:
	cd build; \
	wine python runner.py BLOCK11.exe the_fast_one_376000_2x2x2.csv -r -s

runner-linux-comb:
	cd build; \
	wine python 2023_runnerB.py BLOCK11.exe the_combinatorial_one_42000000_14x10x12.csv -r -s

runner-macos-intro:
	cd build; \
	wine64 python runner.py BLOCK11.exe the_intro_one_32768_4x4x4.csv -r -s

runner-macos-fast:
	cd build; \
	wine64 python runner.py BLOCK11.exe the_fast_one_376000_2x2x2.csv -r -s

runner-macos-comb:
	cd build; \
	wine64 python 2023_runnerB.py BLOCK11.exe the_combinatorial_one_42000000_14x10x12.csv -r -s