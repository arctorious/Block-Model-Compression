/**
 * @file main.cpp
 * @brief Main entry point for the 3D structure compression program.
 *
 * This file contains the main function that initializes the StreamProcessor
 * class and starts the processing of the input stream for compressing 3D structures.
 */

#include <fstream>
#include <iostream>
#include "stream_processor.h"

/**
 * @brief Main function to run the 3D structure compression program.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return Returns 0 on successful execution, 1 if incorrect usage.
 *
 * The main function takes the input file name as a command-line argument,
 * initializes the StreamProcessor object, and starts the stream processing.
 * The input file should contain the uncompressed 3D structure data.
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::string file_name = argv[1];

    StreamProcessor myStreamProcessor(file_name); // Initialize and process the stream

    return 0;
}
