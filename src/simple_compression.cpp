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

    std::cout<<"processing block with bounds:"<<std::endl;
    std::cout<<"x_start: "<<x_start<<" y_start: "<<y_start<<" z_start: "<<z_start<<std::endl;
    std::cout<<"x_end: "<<x_end<<" y_end: "<<y_end<<" z_end: "<<z_end<<std::endl;
    std::cout<<std::endl;

    // Implement the specific compression algorithm for a block here


    // For this class it should process each cell 1 by 1 as said in the lecture

    /* Please only work within the bounds of the parent block
     *      That is only from (x_start,y_start,z_start)->(x_end,y_end,z_end)
     *      Write your output to stdout
    */

    // Good luck team
}
