/**
 * @file stream_processor.h
 * @brief StreamProcessor class to manage the processing of streams with compression, dimensions, and slices.
 *
 * This class provides functionalities for reading and processing stream data,
 * including reading configuration, handling slices, and working with compression.
 * It serves as the main interface for managing the entire stream processing workflow.
 */

#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include "compression.h"
#include "runlength_encoding.h"
#include "simple_compression.h"
#include "octree_compression.h"
#include "runlength_encoding_3D.h"
#include "Decomp3D.h"
#include "dimensions.hpp"
#include "dp_compression.h"

/**
 * @class StreamProcessor
 * @brief Manages the processing of streams with compression, dimensions, and slices.
 *
 * This class provides functionalities for reading and processing stream data,
 * including reading configuration, handling slices, and working with compression.
 */
class StreamProcessor
{
public:
    /**
     * @brief Constructs a StreamProcessor object.
     * 
     * @param
     * Initializes the StreamProcessor with default values.
     */
    StreamProcessor(char alg);

    /**
     * @brief Starts processing the input from standard input.
     *
     * Calls other functions which store the input into data structures
     */
    long double StartProcessing();

    /**
     * @brief Creates a compression algorithm object based on the given name.
     *
     * This function utilizes the Factory Design Pattern to create a compression algorithm object.
     * Different algorithms can be created based on the input name, and a default algorithm is used
     * if the name does not match any known algorithm.
     *
     * @param name The name or identifier for the desired compression algorithm.
     * @return A pointer to the created compression algorithm object.
     */
    Compression* createCompressionAlgorithm(const char name);

    /**
     * @brief Reads the configuration from a specified source.
     *
     * Configures the StreamProcessor according to the provided configuration data.
     */
    void ReadConfiguration();

    /**
     * @brief Reads slices from the stream.
     *
     * Processes and stores slices from the input stream.
     */
    void ReadSlices();

    /**
     * @brief Destructor to clean up the pointer
     *
     * Cleans up the Compression object
     */
    ~StreamProcessor(); // Destructor to clean up the pointer

private:
    // std::ifstream myFin;      ///< Input file stream for reading data.  
    Compression* myCompressor; ///< Object for handling compression.
    std::vector<std::vector<std::vector<char>>> mySlices; ///< Storage for slices data.
    std::unordered_map<char, std::string> myTagTable; ///< Table for mapping tags.
    Dimensions myDimensions; ///< Object for storing dimensions.
    int numSlices = 0;  ///< Integer for keeping track of the number of slices read.
};
