#include <iostream>
#include "runlength_encoding.h"

RunLengthEncoding::RunLengthEncoding(std::vector<std::vector<std::vector<char>>>* Slices,
                                     std::unordered_map<char, std::string>* TagTable,
                                     Dimensions* Dimensions)
    : Compression(Slices, TagTable, Dimensions)
{}


void RunLengthEncoding::CompressBlock(int x_start, int y_start, int z_start) {

    // Specifying the end indexes of this parent block
    int x_end = x_start + myDimensions->x_parent;
    int y_end = y_start + myDimensions->y_parent;
    int z_end = z_start + myDimensions->z_parent;
    

    
    // Implement the specific compression algorithm for a block here

    //i = depth, j = vertical, k = horizontal

    //TODO: ask if y_start can == y_end
    for (int z = z_start; z < z_end; z++){
        for (int y = y_start; y < y_end; y++){
            char saved_key = (*mySlices)[z][y][x_start];
            int key_count = 1;
            int starting_x = x_start;

            for (int x = x_start+1; x < x_end; x++){
                char current_key = (*mySlices)[z][y][x];
                if(saved_key == current_key)
                    key_count++;
                else{
                    PrintOutput(starting_x, y, z + current_level, key_count, 1, 1, getTag(saved_key));
    
                    saved_key = current_key;
                    starting_x = x;
                    key_count = 1;                    
                }
            }
            PrintOutput(starting_x, y, z + current_level, key_count, 1, 1, getTag(saved_key));
        }
    }

}
