/**
 * @file dp_compression.h
 * @brief DynamicProgrammingCompression class to encode input.
 *
 * This class provides a implementation of a iterative dynamic programming method to compress the input.
 * It handles the encoding of individual blocks in the 3D structure.
 */

#pragma once
#include "compression.h"


/**
 * @struct NeighboringSameKeyStreaks
 * @brief Struct to store the neighboring same key streaks.
 *
 * This struct stores the neighboring same key streaks for a given node.
 */
struct NeighboringSameKeyStreaks {
    int left, up, inward;
};

/**
 * @struct SubBlock
 * @brief Struct to store the best sub block.
 *
 * This struct stores the best sub block for a given node.
 */
struct SubBlock {
    int left, up, inward;
};

/**
 * @struct DPNode
 * @brief Struct to store the DP node.
 *
 * This struct stores the DP node for a given node.
 */
struct DPNode {
    NeighboringSameKeyStreaks neighbours;
    SubBlock sub_block;
};


/**
 * @class DynamicProgrammingCompression
 * @brief Derived class for basic 3D structure compression.
 *
 * This class provides an Iterative Dynamic Programming compression algorithm by extending the base Compression class.
 * It is responsible for compressing individual blocks within the 3D structure.
 */
class DynamicProgrammingCompression : public Compression
{
private:

public:
    /**
     * @brief Constructs a runlength_encoding object.
     * 
     * @param Slices Pointer to the slices data structure.
     * @param TagTable Pointer to the tag table mapping.
     * @param Dimensions Pointer to the dimensions object.
     * Initializes the runlength_encoding with the provided parameters.
     */
    DynamicProgrammingCompression(std::vector<std::vector<std::vector<char>>>* Slices,
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
     * @brief Finds the neighboring same key streaks for a given node.
     *
     * @param i The i coordinate of the node.
     * @param j The j coordinate of the node.
     * @param k The k coordinate of the node.
     * @param dp The dynamic programming table.
     * @return NeighboringSameKeys The neighboring same key streaks.
     */
    NeighboringSameKeyStreaks getNeighbouringSameKeyStreaks(int i, int j, int k, const std::vector<std::vector<std::vector<DPNode>>>& dp);

    /**
     * @brief Finds the best sub block for a given node.
     *
     * @param i The i coordinate of the node.
     * @param j The j coordinate of the node.
     * @param k The k coordinate of the node.
     * @param dp The dynamic programming table.
     * @return SubBlock The best sub block.
     */
    SubBlock findBestSubBlock(int i, int j, int k, const std::vector<std::vector<std::vector<DPNode>>>& dp);

    /**
     * @brief Prints the subblocks created inside the dynamic programming table.
     *
     * @param dp The dynamic programming table.
     */
    void PrintDPTableSubBlocks(std::vector<std::vector<std::vector<DPNode>>>& dp);

    /**
     * @brief Prints the neighbours created inside the dynamic programming table.
     *
     * @param dp The dynamic programming table.
     */
    void PrintDPTableNeighbours(std::vector<std::vector<std::vector<DPNode>>>& dp);
    
    /**
     * @brief Backtracks the dynamic programming table to print the (optimal? Don't know yet) subblocks.
     *
     * @param z_start The starting z coordinate of the block.
     * @param x_start The starting x coordinate of the block.
     * @param y_start The starting y coordinate of the block.
     * @param x_end The ending x coordinate of the block.
     * @param y_end The ending y coordinate of the block.
     * @param z_end The ending z coordinate of the block.
     * @param dp The dynamic programming table.
     */
    void startSectioning(int z_start, int x_start, int y_start, int x_end, int y_end, int z_end, const std::vector<std::vector<std::vector<DPNode>>>& dp, std::string label);
};
