#include "runlength_encoding_3D.h"
#include <algorithm>
#include <unordered_set>
#include <iostream>
#include <stack>


struct Block
{
    int myXlen;
    int myYlen;
    int myZlen;
    char myType;
    Block(int Xlen, int Ylen, int Zlen) : myXlen(Xlen), myYlen(Ylen), myZlen(Zlen), myType(' '){}
    Block(int Xlen, int Ylen, int Zlen, char Type) : myXlen(Xlen), myYlen(Ylen), myZlen(Zlen), myType(Type){}
};


Runlength3D::Runlength3D(std::vector<std::vector<std::vector<char>>>* Slices,
                      std::unordered_map<char, std::string>* TagTable,
                      Dimensions* Dimensions) 
    : Compression(Slices,TagTable,Dimensions)
{}

void Runlength3D::PrintBlock(OutputNode& Node)
{
    PrintOutput(Node.myXStart,Node.myYStart,Node.myZStart + current_level,Node.xLen,Node.yLen,Node.zLen,(*myTagTable)[Node.type]);
}

// Define a custom hash function for OutputNode
// instead of hash, keep track of ranges covered
struct OutputNodeHash 
{
    std::size_t operator()(const OutputNode& node) const 
    {

        std::size_t h1 = std::hash<int>{}(node.myXStart);
        std::size_t h2 = std::hash<int>{}(node.myYStart);
        std::size_t h3 = std::hash<int>{}(node.myZStart);
        std::size_t h4 = std::hash<char>{}(node.type);
        return h1 ^ h2 ^ h3 ^ h4;
    }
};

//returns true if A is contained in B.
bool isContainedIn(OutputNode& A, OutputNode& B)
{
    return (A.myXStart >= B.myXStart 
    && A.myYStart >= B.myYStart 
    && A.myZStart >= B.myZStart)
    && (A.myXStart + A.xLen <= B.myXStart + B.xLen
    && A.myYStart + A.yLen <= B.myYStart + B.yLen
    && A.myZStart + A.zLen <= B.myZStart + B.zLen);
}


// Prints all blocks that aren't contained within other blocks in linear time using a monotonic stack.
void Runlength3D::PrintBlocks(std::vector<std::vector<std::vector<OutputNode*>>>& Blocks)
{
    std::stack<OutputNode> monotonicStack;
    std::unordered_set<OutputNode,OutputNodeHash> outputted;
    for(int z = 0; z < myDimensions->z_parent; z++)
    {
        for(int y = 0; y < myDimensions->y_parent; y++)
        {
            for(int x = 0; x < myDimensions->x_parent; x++)
            {
                auto& block = *Blocks[z][y][x];
                if(outputted.find(block) == outputted.end())
                {
                    // PrintBlock(block);
                    outputted.insert(block);
                    while(!monotonicStack.empty())
                    {
                        if(isContainedIn(monotonicStack.top(),block))
                            monotonicStack.pop();
                        else if(isContainedIn(block,monotonicStack.top()))
                            break;
                        else
                        {
                            monotonicStack.push(block);
                            break;
                        }
                    }
                    if(monotonicStack.empty())
                        monotonicStack.push(block);
                    x +=  Blocks[x][y][z]->xLen-1;
                }
            }
        }
    }
    while(!monotonicStack.empty())
    {
        PrintBlock(monotonicStack.top());
        monotonicStack.pop();
    }
}

// Map each index in the parent block to a subblock pointer. Update subblocks accordingly as we grow them while iterating through. This is a 2 (technically 3 for memory allocation) pass RLE solution with O(n) time complexity
void Runlength3D::CompressBlock(int x_start, int y_start, int z_start)
{
    // :TODO: Investigate if using a hashmap would get worse performance, if not then use it since it would have less memory usage.
    std::vector<std::vector<std::vector<OutputNode*>>> subBlocks(myDimensions->z_parent, std::vector<std::vector<OutputNode*>>(myDimensions->y_parent, std::vector<OutputNode*>(myDimensions->x_parent,nullptr)));
    
    const auto length1D = myDimensions->x_parent * myDimensions->y_parent * myDimensions->z_parent;
    std::vector<OutputNode> sharedNodes(length1D);

    // initialise the subBlocks.
    int index = 0;
    for(int z = 0; z < myDimensions->z_parent; z++)
    {
        for(int y = 0; y < myDimensions->y_parent; y++)
        {
            for(int x = 0; x < myDimensions->x_parent; x++)
            {
                subBlocks[z][y][x] = &sharedNodes[index++];
            }
        }
    }
    
    for(int z = 0; z < myDimensions->z_parent; z++)
    {
        // Get Slice
        for(int y = 0; y < myDimensions->y_parent; y++)
        {
            //get subsequent xrow and match with previous xrows if adjacent.
            OutputNode* &start = subBlocks[z][y][0];
            start->SetStart(x_start,y+y_start,z+z_start);
            start->type = (*mySlices)[z+z_start][y+y_start][x_start];
            // Get x Blocks.
            for(int x = 1; x <  myDimensions->x_parent; x++)
            {
                char key = (*mySlices)[z+z_start][y+y_start][x+x_start];
                // If next tagtype on row is same as the tagtype of the current sub block, map to that subBlock.
                if(key == start->type) 
                {
                    start->xLen++;
                    subBlocks[z][y][x] = start;
                    // Handle issue when we encounter new block at the end of a row. 
                    if(x != (myDimensions->x_parent-1)) continue;
                }
                // Check if it can be matched with previous row.
                if(y>0)
                {
                    // Issue was that merging doesn't occur as the previous pointers are still pointing to where start was pointing to. 
                    auto& prevRow = (x == myDimensions->x_parent-1) ? subBlocks[z][y-1][x] : subBlocks[z][y-1][x-1];
                    if(prevRow->type == start->type && prevRow->xLen == start->xLen && prevRow->myXStart == start->myXStart)
                    {
                        prevRow->yLen++;
                        subBlocks[z][y][start->myXStart-x_start + start->xLen-1] = prevRow;
                        start = prevRow;
                    }
                    // If could not be matched with previous row, check if subBlock from previous row can be matched to previous slice.
                    // If sub-block can be merged with sub-block from previous slice then merge in the z direction. 
                    if(z > 0)
                    {
                        auto& prevSlice = (x == myDimensions->x_parent-1) ? subBlocks[z-1][y-1][x] : subBlocks[z-1][y-1][x-1];
                        if((prevRow->type       == prevSlice->type)
                        && (prevRow->myXStart   == prevSlice->myXStart)
                        && (prevRow->myYStart   == prevSlice->myYStart) 
                        && (prevRow->xLen       == prevSlice->xLen)
                        && (prevRow->yLen       == prevSlice->yLen))
                        {
                            prevSlice->zLen++;
                            prevRow = prevSlice;
                        }
                    }
                }
                // Initialise the next block. -- this was stuffing up at the case where the last block in a row is different to the rest of the row, so 
                // I had to check that start's type matches the end block before skipping to account for this specific scenario.
                if(x == (myDimensions->x_parent-1) && start->type == key) continue;
                start = subBlocks[z][y][x];
                start->SetStart(x+x_start,y+y_start,z+z_start);
                start->type = key;
            }
        }
    }

    PrintBlocks(subBlocks);
}

