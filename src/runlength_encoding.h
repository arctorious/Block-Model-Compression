/**
 * @file runlength_encoding.h
 * @brief runlength_encoding class to encode input.
 *
 * This class provides a implementation of runlength encoding.
 * It handles the encoding of individual blocks in the 3D structure.
 */

#pragma once
#include "compression.h"

/**
 * @class runlength_encoding
 * @brief Derived class for basic 3D structure compression.
 *
 * This class provides a simple compression algorithm by extending the base Compression class.
 * It is responsible for compressing individual blocks within the 3D structure.
 */
class RunLengthEncoding : public Compression
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
    RunLengthEncoding(std::vector<std::vector<std::vector<char>>>* Slices,
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
