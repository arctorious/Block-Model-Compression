#include <iostream>
#include "simple_compression.h"

SimpleCompression::SimpleCompression(std::vector<std::vector<std::vector<char>>>* Slices,
                                     std::unordered_map<char, std::string>* TagTable,
                                     Dimensions* Dimensions)
    : Compression(Slices, TagTable, Dimensions)
{}

void SimpleCompression::CompressBlock(int z_start, int x_start, int y_start) {

    // Specifying the end indexes of this parent block
    int x_end = x_start + myDimensions->x_parent;
    int y_end = y_start + myDimensions->y_parent;
    int z_end = z_start + myDimensions->z_parent;
    

    
    // Implement the specific compression algorithm for a block here
    for (int i = z_start; i < z_end; i++){
        for (int j = x_start; j < x_end; j++){
            for (int k = y_start; k < y_end; k++){
                char key = (*mySlices)[i][j][k];
                PrintOutput(k, j, i + current_level, 1, 1, 1, getTag(key));
            }
        }
    }

}
