#ifndef SEPARATOR_H
#define SEPARATOR_H

#include <vector>

class Separator
{

public:
    int orientation;
    std::vector<std::vector<int>> indices;
    Separator(int orientation)
    {
        this->orientation = orientation;
    }
};

#endif