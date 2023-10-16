#include <iostream>
#include "octree_compression.h"
#include <algorithm>

OctreeCompression::OctreeCompression(std::vector<std::vector<std::vector<char>>>* Slices,
                                     std::unordered_map<char, std::string>* TagTable,
                                     Dimensions* Dimensions)
    : Compression(Slices, TagTable, Dimensions)
{}

void OctreeCompression::CompressBlock(int x_start, int y_start, int z_start) {

    std::queue<std::tuple<int, int, int, int, int, int>> q;
    q.push(std::make_tuple(z_start,
                           y_start,
                           x_start,
                           z_start + myDimensions->z_parent-1,
                           y_start + myDimensions->y_parent-1,
                           x_start + myDimensions->x_parent-1));
    
    while (!q.empty()) {
        OctreeNode node(q.front(), mySlices);
        q.pop();
        aggregate           (node,    {0, 1, 2, 3, 4, 5, 6, 7})  || // if all sub-blocks are homogeneous with each other
        n_take_one_aggregate(node, q)                            ||     // else, if any of the 2*2*1 blocks can be grouped
        edge_aggregate      (node, q)                            ||         // else, if any of the 2*1*1 edges can be grouped
        segregate           (node, q, {0, 1, 2, 3, 4, 5, 6, 7});                // else, segregate the sub-blocks
    }
}

void OctreeCompression::PrintOutput(OctreeNode& node, std::vector<std::vector<int>> list) {
    for (std::vector<int>& cells : list) {
        std::sort(cells.begin(), cells.end());
        PrintOutput(node.x_starts[cells[0]],                                                                                // x_start
                    node.y_starts[cells[0]],                                                                                // y_start
                    node.z_starts[cells[0]]+current_level,                                                                  // z_start
                    node.x_ends[cells[cells.size() - 1]]-node.x_starts[cells[0]]+1,                                         // x_size
                    node.y_ends[cells[cells.size() - 1]]-node.y_starts[cells[0]]+1,                                         // y_size
                    node.z_ends[cells[cells.size() - 1]]-node.z_starts[cells[0]]+1,                                         // z_size
                    (*myTagTable)[(*mySlices)[node.z_starts[cells[0]]][node.y_starts[cells[0]]][node.x_starts[cells[0]]]]); // tag
    }
}

bool OctreeCompression::aggregate(OctreeNode& node, std::vector<int> cells, bool print) {
    if (node.homogeneity[cells[0]] == false) {
        return false;
    }
    for (long unsigned int i = 1; i < cells.size(); i++) {
        if (!node.homogeneity[cells[i]] || (*mySlices)[node.z_starts[cells[i]]][node.y_starts[cells[i]]][node.x_starts[cells[i]]] !=
                                           (*mySlices)[node.z_starts[cells[0]]][node.y_starts[cells[0]]][node.x_starts[cells[0]]]) {
            return false;
        }
    }
    if (print) {
        PrintOutput(node, {cells});
    }
    return true;
}

bool OctreeCompression::n_take_one_aggregate(OctreeNode& node, std::queue<std::tuple<int, int, int, int, int, int>>& q, int side) {
    std::vector<std::vector<int>>& faces = side == -1 ? sides : side_edges[side];
    int n = faces.size();
    for (int i = 0; i < n; i++) {
        if (aggregate(node, faces[i])                           && // if the current side can be grouped
            ((i < 3 && aggregate(node, faces[n-1-i]))           ||     // then, if the opposite side can be grouped as well
             (side == -1 && n_take_one_aggregate(node, q, 5-i)) ||         // else, if any of the sub-edges of the opposite side can be grouped
             segregate(node, q, faces[n-1-i])))                                // else, segregate the sub-blocks
        return true;
    }
    return false;
}

bool OctreeCompression::edge_aggregate(OctreeNode& node, std::queue<std::tuple<int, int, int, int, int, int>>& q) {
    for (int i = 0; i < 12; i++) {
        if (!aggregate(node, {pair_octants[i][0], pair_octants[i][1]}, false)) {
            continue;
        }
        if (aggregate(node, {pair_octants[i][0], pair_octants[i][2]}, false) && aggregate(node, {pair_octants[i][1], pair_octants[i][5]}, false)) {
            if (aggregate(node, {pair_octants[i][3], pair_octants[i][7]}, false) && aggregate(node, {pair_octants[i][4], pair_octants[i][6]}, false)) {
                PrintOutput(node, {{pair_octants[i][0], pair_octants[i][2]}, {pair_octants[i][1], pair_octants[i][5]}, {pair_octants[i][3], pair_octants[i][7]}, {pair_octants[i][4], pair_octants[i][6]}});
            } else if (aggregate(node, {pair_octants[i][6], pair_octants[i][7]}, false)) {
                PrintOutput(node, {{pair_octants[i][0], pair_octants[i][2]}, {pair_octants[i][1], pair_octants[i][5]}, {pair_octants[i][6], pair_octants[i][7]}, {pair_octants[i][3]}, {pair_octants[i][4]}});
            } else {
                PrintOutput(node, {{pair_octants[i][0], pair_octants[i][2]}, {pair_octants[i][1], pair_octants[i][5]}, {pair_octants[i][3]}, {pair_octants[i][4]}, {pair_octants[i][6]}, {pair_octants[i][7]}});
            }
        } else if (aggregate(node, {pair_octants[i][0], pair_octants[i][4]}, false) && aggregate(node, {pair_octants[i][1], pair_octants[i][3]}, false)) {
            if (aggregate(node, {pair_octants[i][2], pair_octants[i][6]}, false) && aggregate(node, {pair_octants[i][5], pair_octants[i][7]}, false)) {
                PrintOutput(node, {{pair_octants[i][0], pair_octants[i][4]}, {pair_octants[i][1], pair_octants[i][3]}, {pair_octants[i][2], pair_octants[i][6]}, {pair_octants[i][5], pair_octants[i][7]}});
            } else if (aggregate(node, {pair_octants[i][6], pair_octants[i][7]}, false)) {
                PrintOutput(node, {{pair_octants[i][0], pair_octants[i][4]}, {pair_octants[i][1], pair_octants[i][3]}, {pair_octants[i][6], pair_octants[i][7]}, {pair_octants[i][2]}, {pair_octants[i][5]}});
            } else {
                PrintOutput(node, {{pair_octants[i][0], pair_octants[i][4]}, {pair_octants[i][1], pair_octants[i][3]}, {pair_octants[i][2]}, {pair_octants[i][5]}, {pair_octants[i][6]}, {pair_octants[i][7]}});
            }
        } else {
            PrintOutput(node, {{pair_octants[i][0], pair_octants[i][1]}});
            (aggregate(node, {pair_octants[i][2], pair_octants[i][3]}) && ((aggregate(node, {pair_octants[i][4], pair_octants[i][5]}) && (aggregate(node, {pair_octants[i][6], pair_octants[i][7]}) || segregate(node, q, {pair_octants[i][6], pair_octants[i][7]})))    ||
                                                                           (aggregate(node, {pair_octants[i][6], pair_octants[i][7]}) && segregate(node, q, {pair_octants[i][4], pair_octants[i][5]}))                                                                   ||
                                                                           (aggregate(node, {pair_octants[i][4], pair_octants[i][6]}) && (aggregate(node, {pair_octants[i][5], pair_octants[i][7]}) || segregate(node, q, {pair_octants[i][5], pair_octants[i][7]})))    ||
                                                                           (aggregate(node, {pair_octants[i][5], pair_octants[i][7]}) && segregate(node, q, {pair_octants[i][4], pair_octants[i][6]}))))                                                              ||
            (aggregate(node, {pair_octants[i][4], pair_octants[i][5]}) && ((aggregate(node, {pair_octants[i][2], pair_octants[i][3]}) && (aggregate(node, {pair_octants[i][6], pair_octants[i][7]}) || segregate(node, q, {pair_octants[i][6], pair_octants[i][7]})))    ||
                                                                           (aggregate(node, {pair_octants[i][6], pair_octants[i][7]}) && segregate(node, q, {pair_octants[i][2], pair_octants[i][3]}))                                                                   ||
                                                                           (aggregate(node, {pair_octants[i][2], pair_octants[i][6]}) && (aggregate(node, {pair_octants[i][3], pair_octants[i][7]}) || segregate(node, q, {pair_octants[i][3], pair_octants[i][7]})))    ||
                                                                           (aggregate(node, {pair_octants[i][3], pair_octants[i][7]}) && segregate(node, q, {pair_octants[i][2], pair_octants[i][6]}))))                                                              ||
            (aggregate(node, {pair_octants[i][2], pair_octants[i][6]}) && ((aggregate(node, {pair_octants[i][3], pair_octants[i][7]}) && segregate(node, q, {pair_octants[i][4], pair_octants[i][5]}))                                                                   ||
                                                                           (aggregate(node, {pair_octants[i][5], pair_octants[i][7]}) && segregate(node, q, {pair_octants[i][3], pair_octants[i][4]}))                                                                   ||
                                                                           segregate(node, q, {pair_octants[i][3], pair_octants[i][4], pair_octants[i][5], pair_octants[i][7]})))                                                                                     ||
            (aggregate(node, {pair_octants[i][3], pair_octants[i][7]}) && ((aggregate(node, {pair_octants[i][4], pair_octants[i][6]}) && segregate(node, q, {pair_octants[i][2], pair_octants[i][5]}))                                                                   ||
                                                                           segregate(node, q, {pair_octants[i][2], pair_octants[i][4], pair_octants[i][5], pair_octants[i][6]})))                                                                                     ||
            (aggregate(node, {pair_octants[i][4], pair_octants[i][6]}) && ((aggregate(node, {pair_octants[i][5], pair_octants[i][7]}) && segregate(node, q, {pair_octants[i][2], pair_octants[i][3]}))                                                                   ||
                                                                           segregate(node, q, {pair_octants[i][2], pair_octants[i][3], pair_octants[i][5], pair_octants[i][7]})))                                                                                     ||
            (aggregate(node, {pair_octants[i][5], pair_octants[i][7]}) && segregate(node, q, {pair_octants[i][2], pair_octants[i][3], pair_octants[i][4], pair_octants[i][6]}))                                                                                       ||
            (aggregate(node, {pair_octants[i][6], pair_octants[i][7]}) && segregate(node, q, {pair_octants[i][2], pair_octants[i][3], pair_octants[i][4], pair_octants[i][5]}))                                                                                       ||
            segregate(node, q, {pair_octants[i][2], pair_octants[i][3], pair_octants[i][4], pair_octants[i][5], pair_octants[i][6], pair_octants[i][7]});
        }
        return true;
    }
    return false;
}

bool OctreeCompression::segregate(OctreeNode& node, std::queue<std::tuple<int, int, int, int, int, int>>& q, std::vector<int> cells) {
    for (long unsigned int i = 0; i < cells.size(); i++) {
        if (!aggregate(node, {cells[i]})) {
            q.push(node.get_tuple(cells[i]));
        }
    }
    return true;
}
