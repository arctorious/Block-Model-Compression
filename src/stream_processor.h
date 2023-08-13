#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "compression.h"
#include "dimensions.hpp"

class StreamProcessor
{
    public:
        void StreamProcessor();
        void ReadCofiguration();
        void ReadSlices();

    private:
        Compression myCompressor;
        std::ifstream myFin;
        std::vector<std::vector<std::vector<char>>> mySlices;
        std::unordered_map<char,std::string> myTagTable;
        Dimensions myDimensions;
        

};