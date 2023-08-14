#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include "dimensions.hpp"

class Compression
{
public:
    Compression(std::vector<std::vector<std::vector<char>>>* Slices,
                std::unordered_map<char, std::string>* TagTable,
                Dimensions* Dimensions);
    void Compress(int z);
    virtual void CompressBlock(std::vector<int>& Indices) = 0; // Pure virtual function

private:
    void WorkerFunction(); // Function that each worker thread runs
    void ProcessChunk(int x_start, int y_start, int z_start);

    int x_start;
    int y_start;
    int z_start;

    std::queue<std::vector<int>> workQueue; // Queue to hold chunks to be processed
    std::vector<std::thread> threadPool; // Thread pool
    std::mutex mtx;
    std::condition_variable cv; // Condition variable to signal work
    bool done = false; // Flag to signal that all work is done

    std::vector<std::vector<std::vector<char>>>* mySlices;
    std::unordered_map<char, std::string>* myTagTable;
    Dimensions* myDimensions;
};