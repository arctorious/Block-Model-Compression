#pragma once
#include "compression.h"
#include "dimensions.hpp"
#include "output_node.hpp"


class Runlength3D : public Compression
{
    public: 
        Runlength3D(std::vector<std::vector<std::vector<char>>>* Slices,
                      std::unordered_map<char, std::string>* TagTable,
                      Dimensions* Dimensions);
        void CompressBlock(int x_start, int y_start, int z_start);

    private:
        void PrintBlock(OutputNode& Node) ;
        void PrintBlocks(std::vector<std::vector<std::vector<OutputNode*>>>& Blocks);

};