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
    
    std::vector<std::vector<std::vector<int>>> indices;
    
    std::queue<std::tuple<int, int, int, int, int, int>> q;
    q.push(std::make_tuple(z_start, y_start, x_start, z_end, y_end, x_end));
    
    while (!q.empty()) {
        std::tie(z_start, y_start, x_start, z_end, y_end, x_end) = q.front();
        q.pop();
        int z_mid = (z_start + z_end) / 2;
        int y_mid = (y_start + y_end) / 2;
        int x_mid = (x_start + x_end) / 2;
        // indices of the 8 sub-blocks
        indices = {{{z_start, y_start, x_start}, {z_mid, y_mid, x_mid}},
                   {{z_start, y_start, x_mid+1}, {z_mid, y_mid, x_end}},
                   {{z_start, y_mid+1, x_start}, {z_mid, y_end, x_mid}},
                   {{z_start, y_mid+1, x_mid+1}, {z_mid, y_end, x_end}},
                   {{z_mid+1, y_start, x_start}, {z_end, y_mid, x_mid}},
                   {{z_mid+1, y_start, x_mid+1}, {z_end, y_mid, x_end}},
                   {{z_mid+1, y_mid+1, x_start}, {z_end, y_end, x_mid}},
                   {{z_mid+1, y_mid+1, x_mid+1}, {z_end, y_end, x_end}}};
        // homogeneity of the 8 sub-blocks
        bool homogeneity[8];
        for (int i = 0; i < 8; i++) {
            homogeneity[i] = isHomogeneous(indices[i][0][0], indices[i][0][1], indices[i][0][2], indices[i][1][0], indices[i][1][1], indices[i][1][2]);
        }
        // special cases
        if (aggregate(indices, homogeneity, {0, 1, 2, 3, 4, 5, 6, 7}) || // check if all sub-blocks are homogeneous with each other
            n_take_one_aggregate(indices, homogeneity, q)) {             // n-take-one aggregation
            continue;
        }
        // process all sub-blocks individually
        for (int i = 0; i < 8; i++) {
            if (!aggregate(indices, homogeneity, {i})) {
                q.push(std::make_tuple(indices[i][0][0], indices[i][0][1], indices[i][0][2], indices[i][1][0], indices[i][1][1], indices[i][1][2]));
            }
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

bool OctreeCompression::aggregate(std::vector<std::vector<std::vector<int>>>& indices, bool* homogeneity, std::vector<int> cells) {
    for (long unsigned int i = 0; i < cells.size(); i++) {
        if (!homogeneity[cells[i]] || (*mySlices)[indices[cells[i]][0][0]][indices[cells[i]][0][1]][indices[cells[i]][0][2]] != (*mySlices)[indices[cells[0]][0][0]][indices[cells[0]][0][1]][indices[cells[0]][0][2]]) {
            return false;
        }
    }
    PrintOutput(indices[cells[0]][0][2],                                                                                // x_start
                indices[cells[0]][0][1],                                                                                // y_start
                indices[cells[0]][0][0]+current_level,                                                                  // z_start
                indices[cells[cells.size() - 1]][1][2]-indices[cells[0]][0][2]+1,                                       // x_size
                indices[cells[cells.size() - 1]][1][1]-indices[cells[0]][0][1]+1,                                       // y_size
                indices[cells[cells.size() - 1]][1][0]-indices[cells[0]][0][0]+1,                                       // z_size
                (*myTagTable)[(*mySlices)[indices[cells[0]][0][0]][indices[cells[0]][0][1]][indices[cells[0]][0][2]]]); // tag

    return true;
}

bool OctreeCompression::n_take_one_aggregate(std::vector<std::vector<std::vector<int>>>& indices, bool* homogeneity, std::queue<std::tuple<int, int, int, int, int, int>>& q, int side) {
    std::vector<std::vector<int>>& faces = side == -1 ? sides : side_edges[side];
    int n = faces.size();
    // check for groups of sub-blocks
    for (int i = 0; i < n; i++) {
        if (!aggregate(indices, homogeneity, faces[i])) {
            continue;
        }
        // check if the opposite side can be grouped as well
        if ((i < 3 && aggregate(indices, homogeneity, faces[n-1-i])) ||
            (side == -1 && n_take_one_aggregate(indices, homogeneity, q, 5-i))) {
            return true;
        }
        // if not, process the remaining sub-blocks individually
        for (int j = 0; j < (int)faces[0].size(); j++) {
            if (!aggregate(indices, homogeneity, {faces[n-1-i][j]})) {
                q.push(std::make_tuple(indices[faces[n-1-i][j]][0][0],
                                       indices[faces[n-1-i][j]][0][1],
                                       indices[faces[n-1-i][j]][0][2],
                                       indices[faces[n-1-i][j]][1][0],
                                       indices[faces[n-1-i][j]][1][1],
                                       indices[faces[n-1-i][j]][1][2]));
            }
        }
        return true;
    }
    return false;
}
