#include "octree_node.h"

bool OctreeNode::isHomogeneous(std::vector<std::vector<std::vector<char>>>* mySlices, int i) {
    char first = (*mySlices)[z_starts[i]][y_starts[i]][x_starts[i]];
    for (int z = z_starts[i]; z <= z_ends[i]; z++) {
        for (int y = y_starts[i]; y <= y_ends[i]; y++) {
            for (int x = x_starts[i]; x <= x_ends[i]; x++) {
                if ((*mySlices)[z][y][x] != first) {
                    return false;
                }
            }
        }
    }
    return true;
}

OctreeNode::OctreeNode(std::tuple<int, int, int, int, int, int>& indices, std::vector<std::vector<std::vector<char>>>* mySlices)
    : z_start(std::get<0>(indices)),
      y_start(std::get<1>(indices)),
      x_start(std::get<2>(indices)),
      z_end(std::get<3>(indices)),
      y_end(std::get<4>(indices)),
      x_end(std::get<5>(indices)) {
    int z_mid = (z_start + z_end) / 2;
    int y_mid = (y_start + y_end) / 2;
    int x_mid = (x_start + x_end) / 2;
    z_starts = {z_start, z_start, z_start, z_start, z_mid+1, z_mid+1, z_mid+1, z_mid+1};
    y_starts = {y_start, y_start, y_mid+1, y_mid+1, y_start, y_start, y_mid+1, y_mid+1};
    x_starts = {x_start, x_mid+1, x_start, x_mid+1, x_start, x_mid+1, x_start, x_mid+1};
    z_ends = {z_mid, z_mid, z_mid, z_mid, z_end, z_end, z_end, z_end};
    y_ends = {y_mid, y_mid, y_end, y_end, y_mid, y_mid, y_end, y_end};
    x_ends = {x_mid, x_end, x_mid, x_end, x_mid, x_end, x_mid, x_end};
    for (int i = 0; i < 8; i++) {
        homogeneity.push_back(isHomogeneous(mySlices, i));
    }
}

std::tuple<int, int, int, int, int, int> OctreeNode::get_tuple(int i) {
    return std::make_tuple(z_starts[i], y_starts[i], x_starts[i], z_ends[i], y_ends[i], x_ends[i]);
}
