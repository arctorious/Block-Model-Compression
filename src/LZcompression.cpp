#include "LZcompression.h"
#include <iostream>

LzCompression::LzCompression(
    std::vector<std::vector<std::vector<char>>>* Slices, std::unordered_map<char, 
    std::string>* TagTable,
    Dimensions* Dimensions):Compression(Slices,TagTable,Dimensions)
{
}

void LzCompression::Convert(char Tag, int Permutation, int x_start, int y_start, int z_start)
{
    std::string out;
    for(const OutputNode& output : myCompressions[Permutation])
    {
        PrintOutput(output.xStart+x_start, output.yStart+y_start, output.zStart+z_start, output.xLen, output.yLen, output.zLen,(*myTagTable)[Tag]);

        // :TODO: see if all outputs can be printed at once.
        // out+= std::to_string(output.xStart) + ","+std::to_string(output.yStart)+ ","+std::to_string(output.zStart)+ ","+
        // std::to_string(output.xLen)+ ","+std::to_string(output.yLen)+ ","+std::to_string(output.zLen)+ ","+(*myTagTable)[Tag] + '\n';
        // std::lock_guard<std::mutex> lock(coutMutex);
    }
    std::cout << out;
}

void LzCompression::CompressBlock(int x_start, int y_start, int z_start)
{
    std::unordered_map<char,int> mask;
    for(int z = 0; z < myDimensions->z_parent; z++)
    {
        for(int y = 0; y < myDimensions->y_parent; y++)
        {
            for(int x = 0; x < myDimensions->x_parent; x++)
            {
                char& cell = (*mySlices)[z+z_start][y+y_start][x+x_start];
                mask[cell] |= (1u << (z*myDimensions->x_parent*myDimensions->y_parent + y*myDimensions->x_parent + x));
            }
        }
    }

    for(const auto& tag : mask)
    {
        Convert(tag.first,tag.second,x_start,y_start,z_start);
    }
}

LzCompression::~LzCompression()
{

}