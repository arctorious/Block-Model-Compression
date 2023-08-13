#include <iostream>
#include <fstream>
#include <string>

int main() {

    std::string input_file;
    std::cin>> input_file;
    std::ifstream inputFile(input_file); // Open the file with the given name

    if (!inputFile.is_open()) { // Check if the file was opened successfully
        std::cerr << "Failed to open the file.\n";
        return 1;
    }

    std::string line;
    int input_section = 0;
    while (std::getline(inputFile, line)) { // Read the file line by line
        if (line.empty()) { // Stop reading if an empty line is encountered
            input_section++;
        }
        std::cout << line << '\n'; // Print each line
    }

    inputFile.close(); // Close the file
    return 0;
}
