/**
 * @file simple_compression.h
 * @brief SimpleCompression class to manage basic 3D structure compression.
 *
 * This class provides a simple implementation of the Compression class.
 * It handles the compression of individual blocks in the 3D structure.
 */

#pragma once
#include "compression.h"

/**
 * @class SimpleCompression
 * @brief Derived class for basic 3D structure compression.
 *
 * This class provides a simple compression algorithm by extending the base Compression class.
 * It is responsible for compressing individual blocks within the 3D structure.
 */
class SimpleCompression : public Compression
{
public:
    /**
     * @brief Constructs a SimpleCompression object.
     * 
     * @param Slices Pointer to the slices data structure.
     * @param TagTable Pointer to the tag table mapping.
     * @param Dimensions Pointer to the dimensions object.
     * Initializes the SimpleCompression with the provided parameters.
     */
    SimpleCompression(std::vector<std::vector<std::vector<char>>>* Slices,
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
};
