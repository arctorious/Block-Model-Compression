#include <iostream>
#include "simple_compression.h"

SimpleCompression::SimpleCompression(std::vector<std::vector<std::vector<char>>>* Slices,
                                     std::unordered_map<char, std::string>* TagTable,
                                     Dimensions* Dimensions)
    : Compression(Slices, TagTable, Dimensions)
{}

void SimpleCompression::CompressBlock(int x_start, int y_start, int z_start) {

    // Specifying the end indexes of this parent block
    int x_end = x_start + myDimensions->x_parent;
    int y_end = y_start + myDimensions->y_parent;
    int z_end = z_start + myDimensions->z_parent;
    

    
    // Implement the specific compression algorithm for a block here
    for (int z = z_start; z < z_end; z++){
        for (int y = y_start; y < y_end; y++){
            for (int x = x_start; x < x_end; x++){
                char key = (*mySlices)[z][y][x];
                PrintOutput(x, y, z + current_level, 1, 1, 1, getTag(key));
            }
        }
    }

}
