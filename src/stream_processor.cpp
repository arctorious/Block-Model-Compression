#include <iostream>
#include "stream_processor.h"

StreamProcessor::StreamProcessor(std::string fileName): myFin(fileName),
                                                        myCompressor(&mySlices, &myTagTable, &myDimensions){
    if (!myFin) {
        std::cerr << "Error opening input file." << std::endl;
        return;
    }
    ReadConfiguration();
}

void StreamProcessor::ReadConfiguration(){

    std::string line;
    if (std::getline(myFin, line)) { // Read dimensions
        std::stringstream ss(line);
        std::string token;
        // Tokenize by ',' and populate the struct
        std::getline(ss, token, ','); myDimensions.x_count = std::stoi(token);
        std::getline(ss, token, ','); myDimensions.y_count = std::stoi(token);
        std::getline(ss, token, ','); myDimensions.z_count = std::stoi(token);
        std::getline(ss, token, ','); myDimensions.x_parent = std::stoi(token);
        std::getline(ss, token, ','); myDimensions.y_parent = std::stoi(token);
        std::getline(ss, token, ','); myDimensions.z_parent = std::stoi(token);

        // Output to verify
        std::cout << "x_count: " << myDimensions.x_count << "\n";
        std::cout << "y_count: " << myDimensions.y_count << "\n";
        std::cout << "z_count: " << myDimensions.z_count << "\n";
        std::cout << "x_parent: " << myDimensions.x_parent << "\n";
        std::cout << "y_parent: " << myDimensions.y_parent << "\n";
        std::cout << "z_parent: " << myDimensions.z_parent << "\n";
    }

    while (std::getline(myFin, line)) { // Read Tags
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, ',');
        char key = token[0];
        std::getline(ss, token, ',');
        myTagTable.insert({key, token});

        // Print to verify
        std::cout << "Key: " << key << ", Value: " << token << "\n";
    }

    ReadSlices();
}

void StreamProcessor::ReadSlices(){
    std::vector<std::vector<char>> currentSlice;
    std::string line;

    while (std::getline(myFin, line)) { // Read until the end of the file
        if (line.empty()) {
            // If the line is empty, add the current slice to mySlices and start a new slice
            if (!currentSlice.empty()) {
                mySlices.push_back(currentSlice);
                currentSlice.clear();
                numSlices++;
            }
            // Run the compression algorithm if we have reached maximum number of slices
            if (numSlices == myDimensions.z_parent){
                myCompressor.Compress();
                mySlices.clear();
                currentSlice.clear();
                numSlices = 0;
            }
        } else {
            // If the line is not empty, add it to the current slice
            std::vector<char> row(line.begin(), line.end());
            currentSlice.push_back(row);
        }
    }
    
    // If there's any remaining data in currentSlice, add it to mySlices
    if (!currentSlice.empty()) {
        mySlices.push_back(currentSlice);
    }

    myFin.close();

    // Print to verify
    for (const auto &slice : mySlices) {
        std::cout << "Slice:\n";
        for (const auto &row : slice) {
            for (char ch : row) {
                std::cout << ch;
            }
            std::cout << '\n';
        }
        std::cout << '\n';
    }
}