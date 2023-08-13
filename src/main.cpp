#include <fstream>
#include "stream_processor.h"


int main(int argc, char *argv[]) {
    std::ifstream fin(argv[1]);

    StreamProcessor myStreamProcessor = new StreamProcessor(fin);

    return 0;
}
