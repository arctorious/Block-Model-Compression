#pragma once
#include "compression.h"

class SimpleCompression : public Compression
{
public:
    SimpleCompression(std::vector<std::vector<std::vector<char>>>* Slices,
                      std::unordered_map<char, std::string>* TagTable,
                      Dimensions* Dimensions);

    void CompressBlock(int x_start, int y_start, int z_start) override; // Override the virtual function from Compression
};

