#include "compression.h"

Compression::Compression(std::vector<std::vector<std::vector<char>>> *Slices,
                         std::unordered_map<char, std::string> *TagTable,
                         Dimensions *Dimensions) : mySlices(Slices),
                                                   myTagTable(TagTable),
                                                   myDimensions(Dimensions){};