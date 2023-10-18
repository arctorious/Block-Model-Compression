#include "runlength_encoding_3D.h"
#include <algorithm>
#include <unordered_set>
#include <iostream>
using namespace std;


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
    PrintOutput(Node.myXStart,Node.myYStart,Node.myZStart,Node.xLen,Node.yLen,Node.zLen,(*myTagTable)[Node.type]);
}

// Define a custom hash function for OutputNode
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

void Runlength3D::PrintBlocks(std::vector<std::vector<std::vector<OutputNode*>>>& Blocks)
{
    std::unordered_set<OutputNode, OutputNodeHash> outputted;
    PrintBlock(*Blocks[0][0][0]);
    for(int z = 0; z < myDimensions->z_parent; z++)
    {
        for(int y = 0; y < myDimensions->y_parent; y++)
        {
            for(int x = 0; x < myDimensions->x_parent; x++)
            {
                if(outputted.find(*Blocks[z][y][x]) == outputted.end() && Blocks[z][y][x]->myXStart >= 0)
                {
                    outputted.insert((*Blocks[z][y][x]));
                    PrintBlock(*Blocks[z][y][x]);
                    // x +=  Blocks[x][y][z]->xLen-1;
                }
            }
        }
    }
}

void Runlength3D::CompressBlock(int x_start, int y_start, int z_start)
{
    cout << "Compressing: " << x_start << " " << y_start << " " << z_start << endl;
    // :TODO: Investigate if using a hashmap would get worse performance, if not then use it since it would have less memory usage.
    std::vector<std::vector<std::vector<OutputNode*>>> subBlocks(myDimensions->z_parent, std::vector<std::vector<OutputNode*>>(myDimensions->y_parent, std::vector<OutputNode*>(myDimensions->x_parent,nullptr)));
    
    // Cannot use length until dimensions is casted as const so it can be calculated at compile time.
    // const auto length = myDimensions->x_parent * myDimensions->y_parent * myDimensions->z_parent;
    OutputNode sharedNodes[100];

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
            OutputNode* start = subBlocks[z][y][0];
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
                    if(x != (myDimensions->x_parent-1)) continue;
                }

                // Check if it can be matched with previous row.
                if(y>0)
                {
                    /*
                    AAAA
                    AAAB
                    AAAA
                    */
                    auto& prevRow = subBlocks[z][y-1][x-1];
                    if(prevRow->type == start->type && prevRow->xLen == start->xLen && prevRow->myXStart == start->myXStart)
                    {
                        prevRow->yLen++;
                        start = prevRow;
                    }
                    // If could not be matched with previous row, check if subBlock from previous row can be matched to previous slice.
                    // If sub-block can be merged with sub-block from previous slice then merge in the z direction. This code may cause errors
                    // because it may merge subBlocks twice. 
                    else if( z > 0 
                        && (prevRow->type       == subBlocks[z-1][y-1][x-1]->type)
                        && (prevRow->myXStart   == subBlocks[z-1][y-1][x-1]->myXStart)
                        && (prevRow->myYStart   == subBlocks[z-1][y-1][x-1]->myYStart) 
                        && (prevRow->xLen       == subBlocks[z-1][y-1][x-1]->xLen)
                        && (prevRow->yLen       == subBlocks[z-1][y-1][x-1]->yLen))
                    {
                        subBlocks[z-1][y-1][x-1]->zLen++;
                        subBlocks[z][y-1][x-1] = subBlocks[z-1][y-1][x-1];
                    }
                }
                // Initialise the next block.
                if(x == (myDimensions->x_parent-1)) continue;
                start = subBlocks[z][y][x];
                start->type = key;
            }
        }
    }

    PrintBlocks(subBlocks);
}

