/**
 * @file octree_compression.h
 * @brief OctreeCompression class definition.
 *
 * This class provides a octree implementation of the Compression class.
 */

#pragma once
#include "compression.h"
#include "octree_node.h"

/**
 * @class OctreeCompression
 * @brief Derived class of the Compression class.
 *
 * This class provides a octree compression algorithm by extending the base Compression class.
 */
class OctreeCompression : public Compression
{
private:
    // The octants of each side of the block
    std::vector<std::vector<int>> sides = {{0, 1, 2, 3}, {0, 1, 4, 5}, {0, 2, 4, 6}, {1, 3, 5, 7}, {2, 3, 6, 7}, {4, 5, 6, 7}};
    // The edges of each side of the block
    std::vector<std::vector<std::vector<int>>> side_edges = {{{0, 1}, {0, 2}, {1, 3}, {2, 3}}, 
                                                             {{0, 1}, {0, 4}, {1, 5}, {4, 5}}, 
                                                             {{0, 2}, {0, 4}, {2, 6}, {4, 6}}, 
                                                             {{1, 3}, {1, 5}, {3, 7}, {5, 7}}, 
                                                             {{2, 3}, {2, 6}, {3, 7}, {6, 7}}, 
                                                             {{4, 5}, {4, 6}, {5, 7}, {6, 7}}};
    // The 12 edges of the block
    std::vector<std::vector<int>> pairs = {{0, 1}, {0, 2}, {0, 4}, {1, 3}, {1, 5}, {2, 3}, {2, 6}, {3, 7}, {4, 5}, {4, 6}, {5, 7}, {6, 7}};
    // The octants of the block in relation to each pair
    std::vector<std::vector<int>> pair_octants = {{0, 1, 2, 3, 4, 5, 6, 7},
                                                  {0, 2, 4, 6, 1, 3, 5, 7},
                                                  {0, 4, 1, 5, 2, 6, 3, 7},
                                                  {1, 3, 0, 2, 5, 7, 4, 6},
                                                  {1, 5, 3, 7, 0, 4, 2, 6},
                                                  {2, 3, 6, 7, 0, 1, 4, 5},
                                                  {2, 6, 0, 4, 3, 7, 1, 5},
                                                  {3, 7, 2, 6, 1, 5, 0, 4},
                                                  {4, 5, 0, 1, 6, 7, 2, 3},
                                                  {4, 6, 5, 7, 0, 2, 1, 3},
                                                  {5, 7, 1, 3, 4, 6, 0, 2},
                                                  {6, 7, 4, 5, 2, 3, 0, 1}};

public:
    /**
     * @brief Constructs a OctreeCompression object.
     * 
     * @param Slices Pointer to the slices data structure.
     * @param TagTable Pointer to the tag table mapping.
     * @param Dimensions Pointer to the dimensions object.
     * Initializes the OctreeCompression with the provided parameters.
     */
    OctreeCompression(std::vector<std::vector<std::vector<char>>>* Slices,
                      std::unordered_map<char, std::string>* TagTable,
                      Dimensions* Dimensions);

    /**
     * @brief Compresses an individual block within the 3D structure.
     *
     * @param x_start Starting x coordinate of the block.
     * @param y_start Starting y coordinate of the block.
     * @param z_start Starting z coordinate of the block.
     * Overrides the virtual function from the Compression class.
     */
    void CompressBlock(int x_start, int y_start, int z_start) override;

    using Compression::PrintOutput;
    
    /**
     * @brief Prints the output of the given list of aggregated sub-blocks.
     * @param node 
     * @param list 
     */
    void PrintOutput(OctreeNode& node, std::vector<std::vector<int>> list);
    
    /**
     * @brief Check if the given sub-blocks can be aggregated. If so, output the current block
     * 
     * @param indices the indices of the sub-blocks
     * @param homogeneity the homogeneity of the sub-blocks
     * @param cells the given sub-blocks
     * @return true if the given sub-blocks can be aggregated
     * @return false otherwise
     */
    bool aggregate(OctreeNode& node, std::vector<int> cells, bool print = true);
    
    /**
     * @brief Check if the subblocks of (n-1) dimensions can be aggregated. If so, output the current block
     * 
     * @param indices 
     * @param homogeneity 
     * @param q 
     * @param side 
     * @return true 
     * @return false 
     */
    bool n_take_one_aggregate(OctreeNode& node, std::queue<std::tuple<int, int, int, int, int, int>>& q, int side = -1);
    
    /**
     * @brief Check if any of the edges can be aggregated. If so, output the current block
     * 
     * @param node 
     * @param q 
     * @return true 
     * @return false 
     */
    bool edge_aggregate(OctreeNode& node, std::queue<std::tuple<int, int, int, int, int, int>>& q);
    
    /**
     * @brief Individually output / process the sub-blocks
     * 
     * @param indices 
     * @param homogeneity 
     * @param q 
     * @param cells 
     * @return true 
     * @return false 
     */
    bool segregate(OctreeNode& node, std::queue<std::tuple<int, int, int, int, int, int>>& q, std::vector<int> cells);
};
