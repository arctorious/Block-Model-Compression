/**
 * @file compression.h
 * @brief Compression class to manage 3D structure compression.
 *
 * This class provides the base functionalities for compressing 3D structures.
 * It handles the compression process and supports extension through derived classes.
 */

#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <queue>
#include "dimensions.hpp"

/**
 * @class Compression
 * @brief Base class to manage 3D structure compression.
 *
 * This class provides the essential functionalities for compressing 3D structures.
 * It handles the overall compression process, including dividing the work into chunks
 * and managing the work queue. Derived classes are responsible for implementing the
 * specific compression algorithms by overriding the pure virtual function CompressBlock.
 */
class Compression
{
public:
    /**
     * @brief Constructs a Compression object.
     * 
     * @param Slices Pointer to the slices data structure.
     * @param TagTable Pointer to the tag table mapping.
     * @param Dimensions Pointer to the dimensions object.
     */
    Compression(std::vector<std::vector<std::vector<char>>>* Slices,
                std::unordered_map<char, std::string>* TagTable,
                Dimensions* Dimensions);
    
    /**
     * @brief Starts the compression process.
     *
     * @param z The starting z coordinate.
     */
    void Compress(int z);

    /**
     * @brief Pure virtual function for handling block compression.
     *
     * @param x_start Starting x coordinate of the block.
     * @param y_start Starting y coordinate of the block.
     * @param z_start Starting z coordinate of the block.
     */
    virtual void CompressBlock(int x_start, int y_start, int z_start) = 0;

    /**
     * @brief Virtual destructor.
     */
    virtual ~Compression() = default;

protected:
    std::vector<std::vector<std::vector<char>>>* mySlices; ///< Pointer to the slices data structure.
    std::unordered_map<char, std::string>* myTagTable; ///< Pointer to the tag table mapping.
    Dimensions* myDimensions; ///< Pointer to the dimensions object.

private:
    int x_start; ///< Starting x coordinate.
    int y_start; ///< Starting y coordinate.
    int z_start; ///< Starting z coordinate.

    std::queue<std::vector<int>> workQueue; ///< Queue to hold blocks to be processed.
};