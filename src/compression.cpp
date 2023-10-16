#include "compression.h"
#include <thread>
#include <iostream>

Compression::Compression(std::vector<std::vector<std::vector<char>>>* Slices,
                         std::unordered_map<char, std::string>* TagTable,
                         Dimensions* Dimensions) 
    : mySlices(Slices), myTagTable(TagTable), myDimensions(Dimensions)
{}

void Compression::Compress(int z_start) {
    int x_blocks = myDimensions->x_count/myDimensions->x_parent;
    int y_blocks = myDimensions->y_count/myDimensions->y_parent;
    int x_start = 0, y_start = 0;
    current_level = z_start;

    // Divide the work into chunks and add to the work queue
    for (int y = 0; y < y_blocks; y++) {
        for (int x = 0; x < x_blocks; x++){
            workQueue.push({x_start, y_start, 0});
            x_start += myDimensions->x_parent;
        }
        x_start = 0;
        y_start += myDimensions->y_parent;
    }

    // // Create and launch threads
    // unsigned num_threads = std::thread::hardware_concurrency(); // Number of available hardware threads
    // std::vector<std::thread> threads;
    // for (unsigned i = 0; i < num_threads; ++i) {
    //     threads.emplace_back(&Compression::WorkerFunction, this); // Use the new member function
    // }

    // // Join the threads
    // for (auto& t : threads) {
    //     t.join();
    // }

    WorkerFunction();

}

void Compression::WorkerFunction() {
    std::vector<int> chunk_pos;
    while (!workQueue.empty()) {
        {
            // std::unique_lock<std::mutex> lock(workQueueMutex);
            // if (workQueue.empty()) break; // No more work
            chunk_pos = workQueue.front();
            workQueue.pop();
        }
        CompressBlock(chunk_pos[0], chunk_pos[1], chunk_pos[2]);
    }
}

std::string Compression::getTag(char key){
    return (myTagTable->find(key))->second;
}

void Compression::PrintOutput(int x_position, int y_position, int z_position, int x_size, int y_size, int z_size, const std::string& label) {
    std::lock_guard<std::mutex> lock(coutMutex);
    std::cout << x_position << "," << y_position << "," << z_position << "," << x_size << "," << y_size << "," << z_size << "," << label << '\n';
}