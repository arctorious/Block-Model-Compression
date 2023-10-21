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
    unsigned num_threads = std::thread::hardware_concurrency(); // Number of available hardware threads
    std::vector<std::thread> threads;
    for (unsigned i = 0; i < num_threads; ++i) {
        threads.emplace_back(&Compression::WorkerFunction, this); // Use the new member function
    }

    // Join the threads
    for (auto& t : threads) {
        t.join();
    }

}

void Compression::WorkerFunction() {
    std::vector<int> chunk_pos;
    while (!workQueue.empty()) {
        {
            std::unique_lock<std::mutex> lock(workQueueMutex);
            if (workQueue.empty()) break; // No more work
            chunk_pos = workQueue.front();
            workQueue.pop();
        }
        CompressBlock(chunk_pos[0], chunk_pos[1], chunk_pos[2]);

       {
        
        std::lock_guard<std::mutex> lock(coutMutex);

        // Checking compressed parent block volume matches original parent block volume

        int volume = myDimensions->x_parent * myDimensions->y_parent * myDimensions->z_parent;
        
        if(volume_tracker[std::this_thread::get_id()] != volume){
            std::cout << "Error: Cumulative volume of compressed parent block output is incorrect for thread: "<<std::this_thread::get_id()<<"\n";
            std::cout << volume << " vs "<< volume_tracker[std::this_thread::get_id()] << "\n";
            exit(1);
        }
        else{
            volume_tracker[std::this_thread::get_id()] = 0;
        }

       }
        

    }
}

std::string Compression::getTag(char key){
    return (myTagTable->find(key))->second;
}

void Compression::PrintOutput(int x_position, int y_position, int z_position, int x_size, int y_size, int z_size, const std::string& label) {
    std::lock_guard<std::mutex> lock(coutMutex);

    volume_tracker[std::this_thread::get_id()] += x_size * y_size * z_size;

    std::cout << x_position << "," << y_position << "," << z_position << "," << x_size << "," << y_size << "," << z_size << "," << label << '\n';

    // Check if compressed block that is about to be printed all have the same tag from mySlices
    int start_z = z_position%myDimensions->z_parent;
    char tag = (*mySlices)[start_z][y_position][x_position];
    
    for(int z = start_z; z<start_z+z_size; z++){
        for(int y = y_position; y<y_position+y_size; y++){
            for(int x = x_position; x<x_position+x_size; x++){
                if ( (*mySlices)[z][y][x] != tag) {
                    std::cout<<"ERROR: Two or more blocks with different tags are trying to be compressed\n";
                    exit(1);
                }
                
            
            }

        }
    }
    numSubBlocks++;
}
