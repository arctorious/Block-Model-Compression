// This file is a test file designed to find the best method for outputting text to terminal

#include <iostream>
#include <fstream>
#include <chrono>
#include <utility>

//COUT
void stdPrintOutput(int x_position, int y_position, int z_position, int x_size, int y_size, int z_size, std::string label) {
    std::cout << x_position << "," << y_position << "," << z_position << "," << x_size << "," << y_size << "," << z_size << "," << label << '\n';
}

//COUT with pre made string (this is bad as we have to make the string every iteration of the loop that calls this function)
void stdPreMade(std::string label){
    std::cout << label << '\n';
}

//PRINTF
 void printOutputPRINTF(int x_position, int y_position, int z_position, int x_size, int y_size, int z_size, std::string label){
    printf("%d,%d,%d,%d,%d,%d,%s\n", x_position, y_position, z_position, x_size, y_size, z_size, label.c_str());
}

int main(){

    std::string results = "results.txt";

    // Create an output file stream and open the file
    std::ofstream testResults(results);

    // Check if the file was opened successfully
    if (!testResults.is_open()) {
        std::cerr << "Error opening the file: " << results << std::endl;
        return 1;
    }

    // Write column headers
    testResults << "Method,Time(ms)" << std::endl;

    int x = 1;
    int y = 3;
    int z = 5;
    int xS = 2;
    int yS = 4;
    int zS = 6;
    std::string labelStd = "stdOutput";


    for( int loops = 0; loops < 5; ++loops){
            // METHOD 1 ==================================================================================
            // Start timer
            auto startStd = std::chrono::high_resolution_clock::now();

            for(int i = 0; i < 10000; ++i){
                stdPrintOutput(x,y,z,xS,yS,zS,labelStd);
            }

            // Stop the timer
            auto stopStd = std::chrono::high_resolution_clock::now();

            // Compute the duration
            auto durationStd = std::chrono::duration_cast<std::chrono::milliseconds>(stopStd - startStd);


            // -------------------------------------------------------------------------------------------
            // METHOD 2 ====================================================================================
                // Start timer
            auto startPreMade = std::chrono::high_resolution_clock::now();

            for(int i = 0; i < 10000; ++i){
                std::string target = std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "," + std::to_string(xS) + "," + std::to_string(yS) + "," + std::to_string(zS) + "," + labelStd;
                stdPreMade(target);
            }

            // Stop the timer
            auto stopPreMade = std::chrono::high_resolution_clock::now();

            // Compute the duration
            auto durationPreMade = std::chrono::duration_cast<std::chrono::milliseconds>(stopPreMade - startPreMade);

            // ================== Here is printF ====================================================================
            
            auto startPrintF = std::chrono::high_resolution_clock::now();

            for(int i = 0; i < 10000; ++i){
                printOutputPRINTF(x,y,z,xS,yS,zS,labelStd);
            }

            auto stopPrintF = std::chrono::high_resolution_clock::now();
            auto durationPrintF = std::chrono::duration_cast<std::chrono::milliseconds>(stopPrintF - startPrintF);

            // ================================= Write to file ========================================

            std::string filename = "outputToFile.txt";

            auto startFile = std::chrono::high_resolution_clock::now();


            // Create an output file stream and open the file
            std::ofstream outputFile(filename);

            // Check if the file was opened successfully
            if (!outputFile.is_open()) {
                std::cerr << "Error opening the file: " << filename << std::endl;
                return 1;
            }

            // Write data to the file
            for(int i = 0; i < 10000; ++i){
                std::string target = std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "," + std::to_string(xS) + "," + std::to_string(yS) + "," + std::to_string(zS) + "," + labelStd;
                outputFile << target << std::endl;
            }

            // Close the file when you're done with it
            outputFile.close();

            auto stopFile = std::chrono::high_resolution_clock::now();
            auto durationFile = std::chrono::duration_cast<std::chrono::milliseconds>(stopFile - startFile);

            // Display results and output to file ==============================================================
            // Enter data into the file (two columns)
            testResults << "std::cout <<," << durationStd.count() << std::endl;
            testResults << "std::cout premade," << durationPreMade.count() << std::endl;
            testResults << "printf," << durationPrintF.count() << std::endl;
            testResults << "fileWrite premade," << durationFile.count() << std::endl;
            
            std::cout << "duration for 10,000 line std::cout using << operator: " << durationStd.count() << std::endl;
            std::cout << "duration for 10,000 line std::cout using premade string: " << durationPreMade.count() << std::endl;
            std::cout << "duration for 10,000 line printF: " << durationPrintF.count() << std::endl;
            std::cout << "duration for 10,000 line file write with premade string: " << durationFile.count() << std::endl;

            // -------------------------------------------------------------------------------------------
    }

    // Close the file when you're done with it
    testResults.close();

    
}