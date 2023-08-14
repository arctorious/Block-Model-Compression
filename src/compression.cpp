#include "compression.h"

Compression::Compression(std::vector<std::vector<std::vector<char>>>* Slices,
                         std::unordered_map<char, std::string>* TagTable,
                         Dimensions* Dimensions) 
    : mySlices(Slices), myTagTable(TagTable), myDimensions(Dimensions)
{}

void Compression::Compress(int z_start) {
    int numBlocks = myDimensions.x_count/myDimensions.x_parent;
    int x_start = 0, y_start = 0;

    // Divide the work into chunks and add to the work queue
    for (int i = 0; i < numBlocks; i++) {
        for (int j = 0; j < numBlocks; j++){
            workQueue.push({x_start, y_start, z_start});
            x_start += myDimensions.x_parent;
        }
        x_start = 0;
        y_start += myDimensions.y_parent;
    }
    std::vector<int> chunk_pos;
    for (int i = 0; i < workQueue.size(); i++){
        chunk_pos = workQueue.front();
        workQueue.pop();
        CompressBlock(chunk_pos[0], chunk_pos[1], chunk_pos[2]);
    }
}