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
        indices = {{{z_start, y_start, x_start}, {z_mid, y_mid, x_mid}},
                   {{z_start, y_start, x_mid+1}, {z_mid, y_mid, x_end}},
                   {{z_start, y_mid+1, x_start}, {z_mid, y_end, x_mid}},
                   {{z_start, y_mid+1, x_mid+1}, {z_mid, y_end, x_end}},
                   {{z_mid+1, y_start, x_start}, {z_end, y_mid, x_mid}},
                   {{z_mid+1, y_start, x_mid+1}, {z_end, y_mid, x_end}},
                   {{z_mid+1, y_mid+1, x_start}, {z_end, y_end, x_mid}},
                   {{z_mid+1, y_mid+1, x_mid+1}, {z_end, y_end, x_end}}};
        bool homogeneity[8];
        for (int i = 0; i < 8; i++) {
            homogeneity[i] = isHomogeneous(indices[i][0][0], indices[i][0][1], indices[i][0][2], indices[i][1][0], indices[i][1][1], indices[i][1][2]);
        }
        if (aggregate(indices, homogeneity, {0, 1, 2, 3, 4, 5, 6, 7})) {
            continue;
        }
        bool done = false;
        std::vector<std::vector<int>> sides = {{0, 1, 2, 3}, {0, 1, 4, 5}, {0, 2, 4, 6}, {1, 3, 5, 7}, {2, 3, 6, 7}, {4, 5, 6, 7}};
        for (int i = 0; i < 6; i++) {
            if (aggregate(indices, homogeneity, sides[i])) {
                if (i >= 3 || !aggregate(indices, homogeneity, {sides[5-i][0], sides[5-i][1], sides[5-i][2], sides[5-i][3]})) {
                    for (int j = 0; j < 4; j++) {
                        if (homogeneity[sides[5-i][j]]) {
                            aggregate(indices, homogeneity, {sides[5-i][j]});
                        } else {
                            q.push(std::make_tuple(indices[sides[5-i][j]][0][0], indices[sides[5-i][j]][0][1], indices[sides[5-i][j]][0][2], indices[sides[5-i][j]][1][0], indices[sides[5-i][j]][1][1], indices[sides[5-i][j]][1][2]));
                        }
                    }
                }
                done = true;
                break;
            }
        }
        if (done) {
            continue;
        }
        for (int i = 0; i < 8; i++) {
            if (homogeneity[i]) {
                aggregate(indices, homogeneity, {i});
            } else {
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
    PrintOutput(indices[cells[0]][0][2],
                indices[cells[0]][0][1],
                indices[cells[0]][0][0]+current_level,
                indices[cells[cells.size() - 1]][1][2]-indices[cells[0]][0][2]+1,
                indices[cells[cells.size() - 1]][1][1]-indices[cells[0]][0][1]+1,
                indices[cells[cells.size() - 1]][1][0]-indices[cells[0]][0][0]+1,
                (*myTagTable)[(*mySlices)[indices[cells[0]][0][0]][indices[cells[0]][0][1]][indices[cells[0]][0][2]]]);

    return true;
}
