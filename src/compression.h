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
    virtual void CompressBlock(int x_start, int y_start, int z_start) = 0; // Pure virtual function

private:
    int x_start;
    int y_start;
    int z_start;

    std::queue<std::vector<int>> workQueue; // Queue to hold blocks to be processed

    std::vector<std::vector<std::vector<char>>>* mySlices;
    std::unordered_map<char, std::string>* myTagTable;
    Dimensions* myDimensions;
};