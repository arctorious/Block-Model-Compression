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
#include <mutex>
#include <map>
#include <thread>
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
     * @brief Worker Function for the threads.
     *
     * threads will be looping here.
     */
    void WorkerFunction();

    /**
     * @brief Pure virtual function for handling block compression.
     *
     * @param x_start Starting x coordinate of the block.
     * @param y_start Starting y coordinate of the block.
     * @param z_start Starting z coordinate of the block.
     */
    virtual void CompressBlock(int x_start, int y_start, int z_start) = 0;

    /** 
     * @brief fetches the tag for a given key
     * 
     * @param key character of a specific key
     * @return the fetched tag string
     */
     std::string getTag(char key);

    /**
     * @brief Function to print the output
     *
     * @param x_position Starting x coordinate of the block.
     * @param y_position Starting y coordinate of the block.
     * @param z_position Starting z coordinate of the block.
     * @param x_size     Size of grouped cells in x direction.
     * @param y_size     Size of grouped cells in y direction.
     * @param z_size     Size of grouped cells in z direction.
     * @param lable      Tag lable from the tag table
     */
    void PrintOutput(int x_position, int y_position, int z_position, int x_size, int y_size, int z_size, const std::string& label);

    int getNumSubBlocks() { return numSubBlocks; } ///< Returns the number of sub-blocks printed.

    /**
     * @brief Virtual destructor.
     */
    virtual ~Compression() = default;

protected:
    std::vector<std::vector<std::vector<char>>>* mySlices; ///< Pointer to the slices data structure.
    std::unordered_map<char, std::string>* myTagTable; ///< Pointer to the tag table mapping.
    Dimensions* myDimensions; ///< Pointer to the dimensions object.
    int current_level; ////< Current level of proccessing with refrence o z_parent.
    std::map<std::thread::id, int> volume_tracker;

private:
    int x_start; ///< Starting x coordinate.
    int y_start; ///< Starting y coordinate.
    int z_start; ///< Starting z coordinate.

    std::mutex workQueueMutex; ///< mutex lock for popping the chunks in queue
    std::mutex coutMutex; ///< Mutex to synchronize writes to std::cout

    std::queue<std::vector<int>> workQueue; ///< Queue to hold blocks to be processed.

    int numSubBlocks = 0; ///< Number of sub-blocks printed.
};