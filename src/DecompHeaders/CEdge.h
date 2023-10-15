#ifndef CEDGE_H
#define CEDGE_H

#include <vector>
/*
Concave edges:
    needs atleast 3 block in the shape of an L to form one,
    to store it, it is possible to store it at the empty block, a
    nd with an 	index to indicate which edge is it for that block.
    I think a potentially better way is to loop through the empty
    space horizontaly and vertically checking for concave edges.
    We would store the edges with an unordered map, and a special
    way of indexing, for the right relative edge of a block, you
    increment the index by 1, for the left relative edge of a block
    you dont change the edge. The 	edge will be stored in a single map,
    accessing the edge will require special handling.
*/

// a single concave edge
class CEdge
{
public:
    int orientation;
    int xStart=-1, yStart=-1, zStart=-1;
    std::vector<int> indices;
    CEdge(int orientation)
    {
        this->orientation = orientation;
    }
};

#endif