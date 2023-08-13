#include "stream_processor.h"

void StreamProcessor::StreamProcessor(std::ifstream file){
    myFin = file;
    myCompressor = new Compression(&mySlices, &myTagTable, &myDimensions);
    ReadCofiguration();
}

void StreamProcessor::ReadCofiguration(){

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

    while (std::getline(fin, line)) { // Read Tags
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, ',');
        char key = token[0];
        std::getline(ss, token, ',');
        myTagTable->insert({key, token});

        // Print to verify
        std::cout << "Key: " << key << ", Value: " << token << "\n";
    }

}

void StreamProcessor::ReadSlices(){
    
}