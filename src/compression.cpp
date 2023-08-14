#include "compression.h"

Compression::Compression(std::vector<std::vector<std::vector<char>>>* Slices,
                         std::unordered_map<char, std::string>* TagTable,
                         Dimensions* Dimensions) 
    : mySlices(Slices), myTagTable(TagTable), myDimensions(Dimensions)
{}

void Compression::Compress(int z_start) {
    int numBlocks = myDimensions.x_count/myDimensions.x_parent;
    int x_start = 0, y_start = 0;

    // Divide the work into chunks and add to the work queue
    for (int i = 0; i < numBlocks; i++) {
        for (int j = 0; j < numBlocks; j++){
            workQueue.push({x_start, y_start, z_start});
            x_start += myDimensions.x_parent;
        }
        x_start = 0;
        y_start += myDimensions.y_parent;
    }

    // Create thread pool
    const int numThreads = std::thread::hardware_concurrency();
    for (int i = 0; i < numThreads; ++i) {
        threadPool.emplace_back(&Compression::WorkerFunction, this);
    }

    // Wait for all threads to finish
    for (auto& t : threadPool) {
        t.join();
    }
}

void Compression::WorkerFunction() {
    while (true) {
        std::vector<int> chunk;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this]() { return !workQueue.empty() || done; });
            if (workQueue.empty() && done) return; // All work is done
            chunk = workQueue.front();
            workQueue.pop();
        }
        CompressBlock(chunk[0], chunk[1], chunk[2]);
    }
}