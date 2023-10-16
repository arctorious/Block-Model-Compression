#include "runlength_encoding_3D.h"
#include <algorithm>
#include <unordered_set>


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
                      Dimensions* Dimensions): Compression(Slices,TagTable,Dimensions){}

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
        std::size_t h4 = std::hash<int>{}(node.xLen);
        std::size_t h5 = std::hash<int>{}(node.yLen);
        std::size_t h6 = std::hash<int>{}(node.zLen);
        std::size_t h7 = std::hash<char>{}(node.type);
        return h1 ^ h2 ^ h3 ^ h4 ^ h5 ^ h6 ^ h7;
    }
};

void Runlength3D::PrintBlocks(std::vector<std::vector<std::vector<OutputNode*>>>& Blocks)
{
    std::unordered_set<OutputNode, OutputNodeHash> outputted;
    for(int z = 0; z < Blocks.size(); z++)
    {
        for(int y = 0; y < Blocks[z].size(); y++)
        {
            for(int x = 0; x < Blocks[x][y].size(); x++)
            {
                if(outputted.find(*Blocks[x][y][z]) == outputted.end())
                {
                    outputted.insert((*Blocks[x][y][z]));
                    PrintBlock(*Blocks[x][y][z]);
                }
                x +=  Blocks[x][y][z]->xLen;
            }
        }
    }
}

void Runlength3D::CompressBlock(int x_start, int y_start, int z_start)
{
    // :TODO: Investigate if using a hashmap would get worse performance, if not then use it since it would have less memory usage.
    std::vector<std::vector<std::vector<OutputNode*>>> subBlocks(myDimensions->z_parent, std::vector<std::vector<OutputNode*>>(myDimensions->y_parent, std::vector<OutputNode*>(myDimensions->x_parent,nullptr)));
    
    // Cannot use length until dimensions is casted as const so it can be calculated at compile time.
    // const auto length = myDimensions->x_parent * myDimensions->y_parent * myDimensions->z_parent;
    OutputNode sharedNodes[64];

    // initialise the subBlocks.
    int index = 0;
    for(int z = 0; z < subBlocks.size(); z++)
    {
        for(int y = 0; y < subBlocks[z].size(); y++)
        {
            for(int x = 0; x < subBlocks[x][y].size(); x++)
            {
                subBlocks[x][y][z] = &sharedNodes[index++];
            }
        }
    }

    
    for(int z = z_start; z < z_start + myDimensions->z_parent; z++)
    {
        // Get Slice
        for(int y = y_start; y < y_start + myDimensions->y_parent; y++)
        {
            //get subsequent xrow and match with previous xrows if adjacent.
            OutputNode* start = subBlocks[x_start][y][z];
            start->SetStart(x_start,y,z);
            // Get x Blocks.
            for(int x = x_start+1; x < x_start + myDimensions->x_parent; x++)
            {
                char key = (*mySlices)[z][y][x];
                
                // If next block on row is same as the blocks of the current sub block, map this block to that subBlock.
                if(key == start->type && x != (start->myXStart + myDimensions->x_parent-1)) 
                {
                    subBlocks[x][y][z] = start;
                    continue;
                }

                // New block or end of row. Update the just ended block.
                start->xLen = x - start->myXStart; 

                // Check if it can be matched with previous row.
                if(y>0)
                {
                    if(subBlocks[x][y-1][z]->type == start->type && subBlocks[x][y-1][z]->xLen == start->xLen)
                    {
                        start = subBlocks[x][y-1][z];
                        subBlocks[x][y-1][z]->yLen++;
                    }
                    // If could not be matched with previous row, check if subBlock from previous row can be matched to previous slice.
                    // If sub-block can be merged with sub-block from previous slice then merge in the z direction. This code may cause errors
                    // because it may merge subBlocks twice. 
                    else if( z > 0 
                        && (subBlocks[x][y-1][z]->type == subBlocks[x][y-1][z-1]->type)
                        && (subBlocks[x][y-1][z]->myXStart == subBlocks[x][y-1][z-1]->myXStart)
                        && (subBlocks[x][y-1][z]->myYStart == subBlocks[x][y-1][z-1]->myYStart) 
                        && (subBlocks[x][y-1][z]->myZStart == subBlocks[x][y-1][z-1]->myZStart) 
                        && (subBlocks[x][y-1][z]->xLen == subBlocks[x][y-1][z-1]->xLen)
                        && (subBlocks[x][y-1][z]->yLen == subBlocks[x][y-1][z-1]->yLen))
                    {
                        subBlocks[x][y-1][z-1]->zLen+=subBlocks[x][y-1][z]->zLen;
                        subBlocks[x][y-1][z] = subBlocks[x][y-1][z-1];
                    }
                }
                // Initialise the next block.
                start = subBlocks[x][y][z];
                start->SetStart(x,y,z);
                start->type = key;
            }
        }
    }

    PrintBlocks(subBlocks);
}

