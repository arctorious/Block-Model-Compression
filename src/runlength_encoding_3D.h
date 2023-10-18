#pragma once
#include "compression.h"
#include "dimensions.hpp"
#include "output_node.hpp"


class Runlength3D : public Compression
{
    public: 
        /**
         * @brief Constructs a Runlength3D object.
         * 
         * @param Slices Pointer to the slices data structure.
         * @param TagTable Pointer to the tag table mapping.
         * @param Dimensions Pointer to the dimensions object.
         * Initializes the Runlength3D with the provided parameters.
         */
        
        Runlength3D(std::vector<std::vector<std::vector<char>>>* Slices,
                      std::unordered_map<char, std::string>* TagTable,
                      Dimensions* Dimensions);


        /**
         * @brief Compresses an individual parent block within the 3D structure.
         *
         * @param x_start Starting x coordinate of the block.
         * @param y_start Starting y coordinate of the block.
         * @param z_start Starting z coordinate of the block.
         * Overrides the virtual function from the Compression class.
         */
        void CompressBlock(int x_start, int y_start, int z_start) override;

    private:
        /**
         * @brief Prints an outputNode using the printOutput function
         *
         * @param Node Output node to be printed.
         */
        void PrintBlock(OutputNode& Node) ;

        /**
         * @brief Prints all unique output nodes from the subblocks inside a parent block
         *
         * @param Node Output node to be printed.
         */
        void PrintBlocks(std::vector<std::vector<std::vector<OutputNode*>>>& Blocks);

};