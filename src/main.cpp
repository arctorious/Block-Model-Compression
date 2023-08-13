#include <fstream>
#include <iostream>
#include "stream_processor.h"


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::string file_name = argv[1];

    StreamProcessor myStreamProcessor(file_name);

    return 0;
}
