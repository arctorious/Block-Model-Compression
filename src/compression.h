#pragma once
#include <vector>
#include <unordered_map>
#include <string>

class Compression
{
    public:
        Compression(std::vector<std::vector<std::vector<char>>>* &Slices, std::unordered_map<char,std::string>& TagTable);
        void Compress();
        void CompressBlock(std::vector<int>& Indices);


    private:
        std::vector<std::vector<std::vector<char>>>* mySlices;
        std::unordered_map<char,std::string> myTagTable;
        

};