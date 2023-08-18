#include <iostream>
#include "octree_compression.h"

OctreeCompression::OctreeCompression(std::vector<std::vector<std::vector<char>>>* Slices,
                                     std::unordered_map<char, std::string>* TagTable,
                                     Dimensions* Dimensions)
    : Compression(Slices, TagTable, Dimensions)
{}

void OctreeCompression::CompressBlock(int z_start, int y_start, int x_start) {

    // Specifying the end indexes of this parent block
    int z_end = z_start + myDimensions->z_parent - 1;
    int y_end = y_start + myDimensions->y_parent - 1;
    int x_end = x_start + myDimensions->x_parent - 1;
    
    std::queue<std::tuple<int, int, int, int, int, int>> q;
    q.push(std::make_tuple(z_start, y_start, x_start, z_end, y_end, x_end));
    while (!q.empty()) {
        std::tie(z_start, y_start, x_start, z_end, y_end, x_end) = q.front();
        q.pop();
        if (isHomogeneous(z_start, y_start, x_start, z_end, y_end, x_end)) {
            PrintOutput(x_start, y_start, z_start+current_level, x_end-x_start+1, y_end-y_start+1, z_end-z_start+1, (*myTagTable)[(*mySlices)[z_start][y_start][x_start]]);
        } else {
            int z_mid = (z_start + z_end) / 2;
            int y_mid = (y_start + y_end) / 2;
            int x_mid = (x_start + x_end) / 2;
            
            q.push(std::make_tuple(z_start, y_start, x_start, z_mid, y_mid, x_mid));
            q.push(std::make_tuple(z_start, y_start, x_mid+1, z_mid, y_mid, x_end));
            q.push(std::make_tuple(z_start, y_mid+1, x_start, z_mid, y_end, x_mid));
            q.push(std::make_tuple(z_start, y_mid+1, x_mid+1, z_mid, y_end, x_end));
            q.push(std::make_tuple(z_mid+1, y_start, x_start, z_end, y_mid, x_mid));
            q.push(std::make_tuple(z_mid+1, y_start, x_mid+1, z_end, y_mid, x_end));
            q.push(std::make_tuple(z_mid+1, y_mid+1, x_start, z_end, y_end, x_mid));
            q.push(std::make_tuple(z_mid+1, y_mid+1, x_mid+1, z_end, y_end, x_end));
        }
    }
}

bool OctreeCompression::isHomogeneous(int z_start, int y_start, int x_start, int z_end, int y_end, int x_end) {
    char first = (*mySlices)[z_start][y_start][x_start];
    for (int z = z_start; z <= z_end; z++) {
        for (int y = y_start; y <= y_end; y++) {
            for (int x = x_start; x <= x_end; x++) {
                if ((*mySlices)[z][y][x] != first) {
                    return false;
                }
            }
        }
    }
    return true;
}
