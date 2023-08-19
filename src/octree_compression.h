/**
 * @file octree_compression.h
 * @brief OctreeCompression class definition.
 *
 * This class provides a octree implementation of the Compression class.
 */

#pragma once
#include "compression.h"

/**
 * @class OctreeCompression
 * @brief Derived class of the Compression class.
 *
 * This class provides a octree compression algorithm by extending the base Compression class.
 */
class OctreeCompression : public Compression
{
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
    
    /**
     * @brief Base case: if the current block is homogeneous, output the current block
     * Otherwise, recursively call solve on each of the 8 subblocks
     * 
     * @param z_start Starting z coordinate of the block.
     * @param y_start Starting y coordinate of the block.
     * @param x_start Starting x coordinate of the block.
     * @param z_end Ending z coordinate of the block.
     * @param y_end Ending y coordinate of the block.
     * @param x_end Ending x coordinate of the block.
     */
    void solve(int x_start, int y_start, int z_start, int x_end, int y_end, int z_end);
    
    /**
     * @brief Check if the current block is homogeneous
     * 
     * @param z_start Starting z coordinate of the block.
     * @param y_start Starting y coordinate of the block.
     * @param x_start Starting x coordinate of the block.
     * @param z_end Ending z coordinate of the block.
     * @param y_end Ending y coordinate of the block.
     * @param x_end Ending x coordinate of the block.
     * @return true If the current block is homogeneous
     * @return false If the current block is not homogeneous
     */
    bool isHomogeneous(int x_start, int y_start, int z_start, int x_end, int y_end, int z_end);
    
    /**
     * @brief Check if the given sub-blocks can be aggregated. If so, output the current block
     * 
     * @param indices the indices of the sub-blocks
     * @param homogeneity the homogeneity of the sub-blocks
     * @param cells the given sub-blocks
     * @return true if the given sub-blocks can be aggregated
     * @return false otherwise
     */
    bool aggregate(std::vector<std::vector<std::vector<int>>>& indices, bool* homogeneity, std::vector<int> cells);
};
