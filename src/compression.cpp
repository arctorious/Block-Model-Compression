#include "compression.h"

Compression::Compression(std::vector<std::vector<std::vector<char>>>* Slices,
                         std::unordered_map<char, std::string>* TagTable,
                         Dimensions* Dimensions) 
    : mySlices(Slices), myTagTable(TagTable), myDimensions(Dimensions)
{}

void Compression::Compress(int z_start) {
    int x_blocks = myDimensions->x_count/myDimensions->x_parent;
    int y_blocks = myDimensions->y_count/myDimensions->y_parent;
    int x_start = 0, y_start = 0;
    current_level = z_start;

    // Divide the work into chunks and add to the work queue
    for (int i = 0; i < x_blocks; i++) {
        for (int j = 0; j < y_blocks; j++){
            workQueue.push({0, y_start, x_start});
            x_start += myDimensions->x_parent;
        }
        x_start = 0;
        y_start += myDimensions->y_parent;
    }
    std::vector<int> chunk_pos;

    for (auto i = 0u; i < unsigned (x_blocks*y_blocks); i++){
        chunk_pos = workQueue.front();
        workQueue.pop();
        CompressBlock(chunk_pos[0], chunk_pos[1], chunk_pos[2]);
    }
}