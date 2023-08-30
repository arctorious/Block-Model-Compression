#pragma once

#include <tuple>
#include <vector>

class OctreeNode {
private:
    int z_start, y_start, x_start, z_end, y_end, x_end;

    /**
     * @brief Determine whether the given octant is homogeneous.
     * 
     * @param mySlices 
     * @param i 
     * @return true 
     * @return false 
     */
    bool isHomogeneous(std::vector<std::vector<std::vector<char>>>* mySlices, int i);

public:
    std::vector<int> z_starts, y_starts, x_starts, z_ends, y_ends, x_ends;
    std::vector<bool> homogeneity;

    /**
     * @brief Construct a new Octree Node object
     * 
     * @param indices 
     * @param mySlices 
     */
    OctreeNode(std::tuple<int, int, int, int, int, int>& indices, std::vector<std::vector<std::vector<char>>>* mySlices);

    /**
     * @brief Get a tuple of the indices of the given octant.
     * 
     * @param i 
     * @return std::tuple<int, int, int, int, int, int> 
     */
    std::tuple<int, int, int, int, int, int> get_tuple(int i);
};
