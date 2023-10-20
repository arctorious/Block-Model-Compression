#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <algorithm>
#include <climits>
#include <sstream>
#define xyPlane 0
#define zyPlane 1
#define zxPlane 2

#include "Decomp3D.h"

Decomp3D::Decomp3D(std::vector<std::vector<std::vector<char>>> *Slices,
                   std::unordered_map<char, std::string> *TagTable,
                   Dimensions *Dimensions)
    : Compression(Slices, TagTable, Dimensions)
{
}

void outputSep(std::vector<Separator *> separators)
{
    int sep_idx = 0;
    for (Separator *i : separators)
    {
        std::cout << "Separator #" << sep_idx << "at the ";
        if (i->orientation == xyPlane)
            std::cout << "xyPlane"
                      << "::: ";
        else if (i->orientation == zyPlane)
            std::cout << "zyPlane"
                      << "::: ";
        else if (i->orientation == zxPlane)
            std::cout << "zxPlane"
                      << "::: ";
        for (auto idx : i->indices)
        {
            std::cout << idx[0] << ", " << idx[1] << ", " << idx[2] << " | ";
        }
        std::cout << std::endl;
        sep_idx++;
    }
    std::cout << "end\n";
}

void Decomp3D::CompressBlock(int x_start, int y_start, int z_start)
{
    
    // Specifying the end indexes of this parent block
    int x_end = x_start + myDimensions->x_parent;
    int y_end = y_start + myDimensions->y_parent;
    int z_end = z_start + myDimensions->z_parent;
    for(auto it = (*myTagTable).begin(); it !=(*myTagTable).end(); it++){
        auto blockType = it->first;
        auto tag = it->second;

        // std::cout.flush();
        // std::stringstream out;

        // out << "{";
        // for(int z = z_start; z < z_end; z++){
        //     out << "{";
        //     for(int y = y_start; y < y_end; y++){
        //         out << "{";
        //         for(int x = x_start; x < x_end; x++){
        //             if((*mySlices)[z][y][x] == blockType)
        //                 out << "1";
        //             else
        //                 out << "0";
        //             if(x < x_end-1)
        //                 out << ",";
        //         }

        //         if(y < y_end-1)
        //             out << "},\n";
        //         else
        //             out << "}";
        //     }

        //     if(z < z_end-1)
        //             out << "},\n";
        //         else
        //             out<< "}\n";
        // }
        // out << "},\n";

        // std::cout << out.str() << std::endl;

        // std::cout << "=====COMPRESSING BLOCK: " << blockType << "=====\n";
        std::vector<Block *> blocks = CompressBlockType(x_start, y_start, z_start, x_end, y_end, z_end, blockType);
        // if(blocks.empty()) continue;
        // blockCOunt += blocks.size();

        for(const Block* block : blocks){
            if(block != nullptr) 
            PrintOutput(block->xStart, block->yStart, block->zStart + current_level, block->xDimension, block->yDimension, block->zDimension, tag);
            delete block;   
        }

        

        // std::cout << "BLOCK COUNT : " << blockCOunt << std::endl;
        // std::cout << "=====COMPRESSION FINISHED=====\n";
    }
}

std::vector<Block *> Decomp3D::CompressBlockType(int x_start, int y_start, int z_start, int xLim, int yLim, int zLim, char blockType)
{
    std::vector<Separator *> walls;

    // std::cout << "1. Finding separators"
    //           << "\r\n";
    int_4d edgeMap;
    std::vector<Separator *> separators = findAllSeparators(x_start, y_start, z_start, zLim, yLim, xLim, blockType);
    // std::cout << "2. Finding edges"
    //           << "\n";
    std::vector<CEdge *> edges = findAllConcaveEdges(x_start, y_start, z_start, zLim, yLim, xLim, blockType, edgeMap);

    // std::cout << "=====Entering while loop====="
    //           << "\r\n";
    while (!separators.empty())
    {
        // std::cout << "3. Calculating Weight"
        //           << "\r\n";
        std::vector<int> weights = calWeight(separators, edgeMap);
        // std::cout << "3.1. removing unnecessary separators"
        //           << "\r\n";
        removeUnusedSeparators(weights, separators);

        if (separators.empty())
            break;

        // std::cout << "3.2 Finding maxWeighted Separators"
        //           << "\r\n";
        int maxWeight = INT_MIN;
        for (unsigned int i = 0; i < weights.size(); i++)
        {
            maxWeight = std::max(maxWeight, weights[i]);
        }

        std::vector<Separator *> max_weighted_separators;
        for (unsigned int i = 0; i < weights.size(); i++)
        {
            if (weights[i] == maxWeight)
            {
                max_weighted_separators.push_back(separators[i]);
            }
        }

        // std::cout << "4. Creating graph\r\n";
        std::vector<std::vector<int>> graph = createGraph(max_weighted_separators);

        // std::cout << "5. finding independent set"
        //           << "\r\n";
        std::vector<int> set = maxIS(graph);

        // std::cout << "5.1 Setting Walls"
        //           << "\r\n";
        std::vector<Separator *> finalWalls = setSeparator(max_weighted_separators, set);
        max_weighted_separators.clear();

        for (Separator *sep : finalWalls)
        {
            walls.push_back(sep);
        }

        // std::cout << "5.2 removing walls from the separators vector"
        //           << "\r\n";
        // std::cout << separators.size() << " | " << finalWalls.size() << std::endl;
        removeSeparatorList(separators, finalWalls);
        // std::cout << separators.size() << " | \n";
        // std::cout << "5.3 removing edges erased by the walls"
        //           << "\r\n";


                  
        removeEdges(edges, finalWalls, edgeMap);

        // std::cout << "6. Splitting separators by walls" << "\r\n";
        std::vector<Separator* > newSep = splitSeparators(separators, finalWalls);

        // std::cout << "7. Splitting edges by walls" << "\r\n";
        std::vector<CEdge* > newEdge = splitEdges(edges, finalWalls, edgeMap);


        // std::cout << "__________________________"
        //           << "\r\n";

                  
    }
    // combining
    // std::cout << "=====FINISHED WHILE LOOP=====" << "\r\n";

    // creating the map
    std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, bool>>>> wallMap;

    for (Separator* sep : walls)
    {
        int orientation = sep->orientation;
        for (std::vector<int>& indice : sep->indices)
        {
            int z = indice[0];
            int y = indice[1];
            int x = indice[2];

            wallMap[z][y][x][orientation];
        }
    }

    // std::cout << "8. Clearing variables" << "\r\n";

    // std::cout << "9. forming blocks" << "\r\n";
    std::vector<Block *> blocks = formingBlocks(z_start, y_start, x_start, zLim, yLim, xLim, blockType, wallMap);
    // std::cout << "9.1 merging blocks" << "\r\n";
    mergeAdjacentBlock(blocks);

    // std::cout << "=====algorithm finished=====" << "\r\n";
    return blocks;
}

Separator *Decomp3D::findSingleSeparator_xy(int x, int y, int z, int xLim, int yLim, char blockType, bool_3d &visited)
{
    visited[z][y][x] = true;

    std::queue<std::vector<int>> indexQueue;
    indexQueue.push({x, y});

    Separator *separator = new Separator(xyPlane);
    separator->indices.push_back({z, y, x});

    while (!indexQueue.empty())
    {
        int curX = indexQueue.front()[0];
        int curY = indexQueue.front()[1];
        indexQueue.pop();

        // check if the right side of the current block has a block, if it does, then
        // loop through, if it does not, then dont loop through.

        if (curX + 1 < xLim && visited[z][curY][curX + 1] == false && (*mySlices)[z][curY][curX + 1] == blockType && (*mySlices)[z + 1][curY][curX + 1] == blockType)
        {

            visited[z][curY][curX + 1] = true;
            separator->indices.push_back({z, curY, curX + 1});
            indexQueue.push({curX + 1, curY});
        }

        if (curY + 1 < yLim && visited[z][curY + 1][curX] == false && (*mySlices)[z][curY + 1][curX] == blockType && (*mySlices)[z + 1][curY + 1][curX] == blockType)
        {

            visited[z][curY + 1][curX] = true;
            separator->indices.push_back({z, curY + 1, curX});
            indexQueue.push({curX, curY + 1});
        }

        if (curX - 1 >= 0 && visited[z][curY][curX - 1] == false && (*mySlices)[z][curY][curX - 1] == blockType && (*mySlices)[z + 1][curY][curX - 1] == blockType)
        {

            visited[z][curY][curX - 1] = true;
            separator->indices.push_back({z, curY, curX - 1});
            indexQueue.push({curX - 1, curY});
        }

        if (curY - 1 >= 0 && visited[z][curY - 1][curX] == false && (*mySlices)[z][curY - 1][curX] == blockType && (*mySlices)[z + 1][curY - 1][curX] == blockType)
        {

            visited[z][curY - 1][curX] = true;
            separator->indices.push_back({z, curY - 1, curX});
            indexQueue.push({curX, curY - 1});
        }
    }

    return separator;
}

// vertical
Separator *Decomp3D::findSingleSeparator_zy(int x, int y, int z, int yLim, int zLim, char blockType, bool_3d &visited)
{
    visited[z][y][x] = true;

    std::queue<std::vector<int>> indexQueue;
    indexQueue.push({z, y});

    Separator *separator = new Separator(zyPlane);
    separator->indices.push_back({z, y, x});

    while (!indexQueue.empty())
    {
        int curZ = indexQueue.front()[0];
        int curY = indexQueue.front()[1];
        indexQueue.pop();

        // check if the right side of the current block has a block, if it does, then
        // loop through, if it does not, then dont loop through.

        if (curZ + 1 < zLim && visited[curZ + 1][curY][x] == false && (*mySlices)[curZ + 1][curY][x] == blockType && (*mySlices)[curZ + 1][curY][x + 1] == blockType)
        {

            visited[curZ + 1][curY][x] = true;
            separator->indices.push_back({curZ + 1, curY, x});
            indexQueue.push({curZ + 1, curY});
        }

        if (curY + 1 < yLim && visited[curZ][curY + 1][x] == false && (*mySlices)[curZ][curY + 1][x] == blockType && (*mySlices)[curZ][curY + 1][x + 1] == blockType)
        {

            visited[curZ][curY + 1][x] = true;
            separator->indices.push_back({curZ, curY + 1, x});
            indexQueue.push({curZ, curY + 1});
        }

        if (curZ - 1 >= 0 && visited[curZ - 1][curY][x] == false && (*mySlices)[curZ - 1][curY][x] == blockType && (*mySlices)[curZ - 1][curY][x + 1] == blockType)
        {

            visited[curZ - 1][curY][x] = true;
            separator->indices.push_back({curZ - 1, curY, x});
            indexQueue.push({curZ - 1, curY});
        }

        if (curY - 1 >= 0 && visited[curZ][curY - 1][x] == false && (*mySlices)[curZ][curY - 1][x] == blockType && (*mySlices)[curZ][curY - 1][x + 1] == blockType)
        {

            visited[curZ][curY - 1][x] = true;
            separator->indices.push_back({curZ, curY - 1, x});
            indexQueue.push({curZ, curY - 1});
        }
    }

    return separator;
}

// vertical flat
Separator *Decomp3D::findSingleSeparator_zx(int x, int y, int z, int xLim, int zLim, char blockType, bool_3d &visited)
{
    visited[z][y][x] = true;

    std::queue<std::vector<int>> indexQueue;
    indexQueue.push({z, x});

    Separator *separator = new Separator(zxPlane);
    separator->indices.push_back({z, y, x});

    while (!indexQueue.empty())
    {
        int curZ = indexQueue.front()[0];
        int curX = indexQueue.front()[1];
        indexQueue.pop();

        // check if the right side of the current block has a block, if it does, then
        // loop through, if it does not, then dont loop through.

        if (curZ + 1 < zLim && visited[curZ + 1][y][curX] == false && (*mySlices)[curZ + 1][y][curX] == blockType && (*mySlices)[curZ + 1][y + 1][curX] == blockType)
        {

            visited[curZ + 1][y][curX] = true;
            separator->indices.push_back({curZ + 1, y, curX});
            indexQueue.push({curZ + 1, curX});
        }

        if (curX + 1 < xLim && visited[curZ][y][curX + 1] == false && (*mySlices)[curZ][y][curX + 1] == blockType && (*mySlices)[curZ][y + 1][curX + 1] == blockType)
        {

            visited[curZ][y][curX + 1] = true;
            separator->indices.push_back({curZ, y, curX + 1});
            indexQueue.push({curZ, curX + 1});
        }

        if (curZ - 1 >= 0 && visited[curZ - 1][y][curX] == false && (*mySlices)[curZ - 1][y][curX] == blockType && (*mySlices)[curZ - 1][y + 1][curX] == blockType)
        {

            visited[curZ - 1][y][curX] = true;
            separator->indices.push_back({curZ - 1, y, curX});
            indexQueue.push({curZ - 1, curX});
        }

        if (curX - 1 >= 0 && visited[curZ][y][curX - 1] == false && (*mySlices)[curZ][y][curX - 1] == blockType && (*mySlices)[curZ][y + 1][curX - 1] == blockType)
        {

            visited[curZ][y][curX - 1] = true;
            separator->indices.push_back({curZ, y, curX - 1});
            indexQueue.push({curZ, curX - 1});
        }
    }
    return separator;
}

// create new class as you go to save time?
// returns a vector of separators
std::vector<Separator *> Decomp3D::findAllSeparators(int xStart, int yStart, int zStart, int zLim, int yLim, int xLim, char blockType)
{
    // std::vector<Separator *> separators = findAllSeparators(x_start, y_start, z_start, zLim, yLim, xLim, blockType);

    // 3d map
    bool_3d visited;
    std::vector<Separator *> separators;

    for (int z = zStart; z < zLim - 1; z++)
    {
        for (int y = yStart; y < yLim; y++)
        {
            for (int x = xStart; x < xLim; x++)
            {
                // check if the current block is either: visited, is the same type of block, has a block on the right hand side for an
                // appropriate separator.

                if ((*mySlices)[z][y][x] == blockType && (*mySlices)[z + 1][y][x] == blockType && visited[z][y][x] == false)
                {
                    separators.push_back(findSingleSeparator_xy(x, y, z, xLim, yLim, blockType, visited));
                }
            }
        }
    }

    visited.clear();

    for (int x = xStart; x < xLim - 1; x++)
    {
        for (int y = yStart; y < yLim; y++)
        {
            for (int z = zStart; z < zLim; z++)
            {
                // check if the current block is either: visited, is the same type of block, has a block on the right hand side for an
                // appropriate separator.
                if ((*mySlices)[z][y][x] == blockType && (*mySlices)[z][y][x + 1] == blockType && visited[z][y][x] == false)
                {
                    separators.push_back(findSingleSeparator_zy(x, y, z, yLim, zLim, blockType, visited));
                }
            }
        }
    }

    visited.clear();

    for (int y = yStart; y < yLim - 1; y++)
    {
        for (int x = xStart; x < xLim; x++)
        {
            for (int z = zStart; z < zLim; z++)
            {
                // check if the current block is either: visited, is the same type of block, has a block on the right hand side for an
                // appropriate separator.
                if ((*mySlices)[z][y][x] == blockType && (*mySlices)[z][y + 1][x] == blockType && visited[z][y][x] == false)
                {
                    separators.push_back(findSingleSeparator_zx(x, y, z, xLim, zLim, blockType, visited));
                }
            }
        }
    }

    return separators;
}

CEdge *Decomp3D::findSingleConcaveEdge_xdirection(int &x, int y, int z, int xLim, char blockType)
{
    // check if the current edge in the current index is concave or not.
    // via block orientation
    // check the 4 adjacent block's existance

    if ((*mySlices)[z - 1][y - 1][x] != blockType)
    {
        if ((*mySlices)[z - 1][y][x] == blockType && (*mySlices)[z][y - 1][x] == blockType && (*mySlices)[z][y][x] == blockType)
        {
            CEdge *edge = new CEdge(xyPlane);
            edge->xStart = x;
            edge->yStart = y;
            edge->zStart = z;
            edge->indices.push_back(x);
            x++;
            while (x < xLim)
            {
                if ((*mySlices)[z - 1][y - 1][x] != blockType && (*mySlices)[z - 1][y][x] == blockType && (*mySlices)[z][y - 1][x] == blockType && (*mySlices)[z][y][x] == blockType)
                {
                    edge->indices.push_back(x);
                    x++;
                }
                else
                {
                    break;
                }
            }
            return edge;
        }
    }

    if ((*mySlices)[z - 1][y][x] != blockType)
    {
        if ((*mySlices)[z - 1][y - 1][x] == blockType && (*mySlices)[z][y - 1][x] == blockType && (*mySlices)[z][y][x] == blockType)
        {
            CEdge *edge = new CEdge(xyPlane);
            edge->xStart = x;
            edge->yStart = y;
            edge->zStart = z;
            edge->indices.push_back(x);
            x++;
            while (x < xLim)
            {
                if ((*mySlices)[z - 1][y][x] != blockType && (*mySlices)[z - 1][y - 1][x] == blockType && (*mySlices)[z][y - 1][x] == blockType && (*mySlices)[z][y][x] == blockType)
                {
                    edge->indices.push_back(x);
                    x++;
                }
                else
                {
                    break;
                }
            }

            return edge;
        }
    }
    if ((*mySlices)[z][y - 1][x] != blockType)
    {
        if ((*mySlices)[z - 1][y][x] == blockType && (*mySlices)[z - 1][y - 1][x] == blockType && (*mySlices)[z][y][x] == blockType)
        {
            CEdge *edge = new CEdge(xyPlane);
            edge->xStart = x;
            edge->yStart = y;
            edge->zStart = z;
            edge->indices.push_back(x);
            x++;
            while (x < xLim)
            {
                if ((*mySlices)[z][y - 1][x] != blockType && (*mySlices)[z - 1][y][x] == blockType && (*mySlices)[z - 1][y - 1][x] == blockType && (*mySlices)[z][y][x] == blockType)
                {
                    edge->indices.push_back(x);
                    x++;
                }
                else
                {
                    break;
                }
            }

            return edge;
        }
    }
    if ((*mySlices)[z][y][x] != blockType)
    {
        if ((*mySlices)[z - 1][y][x] == blockType && (*mySlices)[z][y - 1][x] == blockType && (*mySlices)[z - 1][y - 1][x] == blockType)
        {
            CEdge *edge = new CEdge(xyPlane);
            edge->xStart = x;
            edge->yStart = y;
            edge->zStart = z;
            edge->indices.push_back(x);
            x++;
            while (x < xLim)
            {
                if ((*mySlices)[z][y][x] != blockType && (*mySlices)[z - 1][y][x] == blockType && (*mySlices)[z][y - 1][x] == blockType && (*mySlices)[z - 1][y - 1][x] == blockType)
                {
                    edge->indices.push_back(x);
                    x++;
                }
                else
                {
                    break;
                }
            }

            return edge;
        }
    }
    x++;

    return nullptr;
}

CEdge *Decomp3D::findSingleConcaveEdge_ydirection(int x, int &y, int z, int yLim, char blockType)
{
    // check if the current edge in the current index is concave or not.
    // via block orientation
    // check the 4 adjacent block's existance
    if ((*mySlices)[z - 1][y][x - 1] != blockType)
    {
        if ((*mySlices)[z - 1][y][x] == blockType && (*mySlices)[z][y][x - 1] == blockType && (*mySlices)[z][y][x] == blockType)
        {
            CEdge *edge = new CEdge(zyPlane);
            edge->xStart = x;
            edge->yStart = y;
            edge->zStart = z;
            edge->indices.push_back(y);
            y++;
            while (y < yLim)
            {
                if ((*mySlices)[z - 1][y][x - 1] != blockType && (*mySlices)[z - 1][y][x] == blockType && (*mySlices)[z][y][x - 1] == blockType && (*mySlices)[z][y][x] == blockType)
                {
                    edge->indices.push_back(y);
                    y++;
                }
                else
                {
                    break;
                }
            }

            return edge;
        }
    }

    if ((*mySlices)[z - 1][y][x] != blockType)
    {
        if ((*mySlices)[z - 1][y][x - 1] == blockType && (*mySlices)[z][y][x - 1] == blockType && (*mySlices)[z][y][x] == blockType)
        {
            CEdge *edge = new CEdge(zyPlane);
            edge->xStart = x;
            edge->yStart = y;
            edge->zStart = z;
            edge->indices.push_back(y);
            y++;
            while (y < yLim)
            {
                if ((*mySlices)[z - 1][y][x] != blockType && (*mySlices)[z - 1][y][x - 1] == blockType && (*mySlices)[z][y][x - 1] == blockType && (*mySlices)[z][y][x] == blockType)
                {
                    edge->indices.push_back(y);
                    y++;
                }
                else
                {
                    break;
                }
            }
            return edge;
        }
    }
    if ((*mySlices)[z][y][x - 1] != blockType)
    {
        if ((*mySlices)[z - 1][y][x] == blockType && (*mySlices)[z - 1][y][x - 1] == blockType && (*mySlices)[z][y][x] == blockType)
        {
            CEdge *edge = new CEdge(zyPlane);
            edge->xStart = x;
            edge->yStart = y;
            edge->zStart = z;
            edge->indices.push_back(y);
            y++;
            while (y < yLim)
            {
                if ((*mySlices)[z][y][x - 1] != blockType && (*mySlices)[z - 1][y][x] == blockType && (*mySlices)[z - 1][y][x - 1] == blockType && (*mySlices)[z][y][x] == blockType)
                {
                    edge->indices.push_back(y);
                    y++;
                }
                else
                {
                    break;
                }
            }

            return edge;
        }
    }

    if ((*mySlices)[z][y][x] != blockType)
    {
        if ((*mySlices)[z - 1][y][x] == blockType && (*mySlices)[z][y][x - 1] == blockType && (*mySlices)[z - 1][y][x - 1] == blockType)
        {
            CEdge *edge = new CEdge(zyPlane);
            edge->xStart = x;
            edge->yStart = y;
            edge->zStart = z;
            edge->indices.push_back(y);
            y++;
            while (y < yLim)
            {
                if ((*mySlices)[z][y][x] != blockType && (*mySlices)[z - 1][y][x] == blockType && (*mySlices)[z][y][x - 1] == blockType && (*mySlices)[z - 1][y][x - 1] == blockType)
                {
                    edge->indices.push_back(y);
                    y++;
                }
                else
                {
                    break;
                }
            }

            return edge;
        }
    }
    y++;

    return nullptr;
}

CEdge *Decomp3D::findSingleConcaveEdge_zdirection(int x, int y, int &z, int zLim, char blockType)
{
    // check if the current edge in the current index is concave or not.
    // via block orientation
    // check the 4 adjacent block's existance

    if ((*mySlices)[z][y - 1][x - 1] != blockType)
    {
        if ((*mySlices)[z][y][x] == blockType && (*mySlices)[z][y - 1][x] == blockType && (*mySlices)[z][y][x - 1] == blockType)
        {
            CEdge *edge = new CEdge(zxPlane);
            edge->xStart = x;
            edge->yStart = y;
            edge->zStart = z;
            edge->indices.push_back(z);
            z++;
            while (z < zLim)
            {
                if ((*mySlices)[z][y - 1][x - 1] != blockType && (*mySlices)[z][y][x] == blockType && (*mySlices)[z][y - 1][x] == blockType && (*mySlices)[z][y][x - 1] == blockType)
                {
                    edge->indices.push_back(z);
                    z++;
                }
                else
                {
                    break;
                }
            }

            return edge;
        }
    }

    if ((*mySlices)[z][y - 1][x] != blockType)
    {
        if ((*mySlices)[z][y][x] == blockType && (*mySlices)[z][y - 1][x - 1] == blockType && (*mySlices)[z][y][x - 1] == blockType)
        {

            CEdge *edge = new CEdge(zxPlane);
            edge->xStart = x;
            edge->yStart = y;
            edge->zStart = z;
            edge->indices.push_back(z);
            z++;
            while (z < zLim)
            {

                if ((*mySlices)[z][y - 1][x] != blockType && (*mySlices)[z][y][x] == blockType && (*mySlices)[z][y - 1][x - 1] == blockType && (*mySlices)[z][y][x - 1] == blockType)
                {
                    edge->indices.push_back(z);
                    z++;
                }
                else
                {
                    break;
                }
            }

            return edge;
        }
    }

    if ((*mySlices)[z][y][x - 1] != blockType)
    {
        if ((*mySlices)[z][y][x] == blockType && (*mySlices)[z][y - 1][x] == blockType && (*mySlices)[z][y - 1][x - 1] == blockType)
        {
            CEdge *edge = new CEdge(zxPlane);
            edge->xStart = x;
            edge->yStart = y;
            edge->zStart = z;
            edge->indices.push_back(z);
            z++;
            while (z < zLim)
            {
                if ((*mySlices)[z][y][x - 1] != blockType && (*mySlices)[z][y][x] == blockType && (*mySlices)[z][y - 1][x] == blockType && (*mySlices)[z][y - 1][x - 1] == blockType)
                {
                    edge->indices.push_back(z);
                    z++;
                }
                else
                {
                    break;
                }
            }
            return edge;
        }
    }

    if ((*mySlices)[z][y][x] != blockType)
    {
        if ((*mySlices)[z][y - 1][x - 1] == blockType && (*mySlices)[z][y - 1][x] == blockType && (*mySlices)[z][y][x - 1] == blockType)
        {
            CEdge *edge = new CEdge(zxPlane);
            edge->xStart = x;
            edge->yStart = y;
            edge->zStart = z;
            edge->indices.push_back(z);
            z++;
            while (z < zLim)
            {
                if ((*mySlices)[z][y][x] != blockType && (*mySlices)[z][y - 1][x - 1] == blockType && (*mySlices)[z][y - 1][x] == blockType && (*mySlices)[z][y][x - 1] == blockType)
                {
                    edge->indices.push_back(z);
                    z++;
                }
                else
                {
                    break;
                }
            }
            return edge;
        }
    }
    z++;

    return nullptr;
}

// this maps to the index of the edge

std::vector<CEdge *> Decomp3D::findAllConcaveEdges(int xStart, int yStart, int zStart,
                                                   int zLim, int yLim, int xLim, char blockType, int_4d& concaveEdgeMap)
{
    // the outer edges cannot be concave, so it is safe to increment the outer two loops by 1
    // and left the limits unmodified

    std::vector<CEdge *> concaveEdges;

    for (int z = zStart + 1; z < zLim; z++)
    {
        for (int y = yStart + 1; y < yLim; y++)
        {
            int x = xStart;
            while (x < xLim)
            {
                // x will be incremented in this function
                CEdge *edge = findSingleConcaveEdge_xdirection(x, y, z, xLim, blockType);
                if (edge != nullptr)
                {
                    for (int edgePos : edge->indices)
                        concaveEdgeMap[edge->zStart][edge->yStart][edgePos][edge->orientation] = concaveEdges.size();
                    concaveEdges.push_back(edge);
                }
            }
        }
    }

    for (int x = xStart + 1; x < xLim; x++)
    {
        for (int y = yStart + 1; y < yLim; y++)
        {
            int z = zStart;
            while (z < zLim)
            {
                // z will be incremented in this function
                CEdge *edge = findSingleConcaveEdge_zdirection(x, y, z, zLim, blockType);
                if (edge != nullptr)
                {
                    for (int edgePos : edge->indices)
                        concaveEdgeMap[edgePos][edge->yStart][edge->xStart][edge->orientation] = concaveEdges.size();
                    concaveEdges.push_back(edge);
                }
            }
        }
    }

    for (int z = zStart + 1; z < zLim; z++)
    {
        for (int x = xStart + 1; x < xLim; x++)
        {
            int y = yStart;
            while (y < yLim)
            {
                // y will be incremented in this function
                CEdge *edge = findSingleConcaveEdge_ydirection(x, y, z, yLim, blockType);
                if (edge != nullptr)
                {
                    for (int edgePos : edge->indices)
                        concaveEdgeMap[edge->zStart][edgePos][edge->xStart][edge->orientation] = concaveEdges.size();
                    concaveEdges.push_back(edge);
                }
            }
        }
    }

    return concaveEdges;
}

void Decomp3D::insert_to_map(CEdge *edge, int index, int_4d& concaveEdgeMap)
{
    int orientation = edge->orientation;
    int z = edge->zStart;
    int y = edge->yStart;
    int x = edge->xStart;

    if (orientation == xyPlane)
    {
        for (int idx : edge->indices)
        {
            auto itZ = concaveEdgeMap.find(z);
            if (itZ == concaveEdgeMap.end())
            {
                // z doesn't exist, so create an entry for it
                itZ = concaveEdgeMap.insert({z, {}}).first;
            }

            auto itY = itZ->second.find(y);
            if (itY == itZ->second.end())
            {
                // y doesn't exist, so create an entry for it
                itY = itZ->second.insert({y, {}}).first;
            }

            auto itX = itY->second.find(idx);
            if (itX == itY->second.end())
            {
                // X doesn't exist, so create an entry for it
                itX = itY->second.insert({idx, {}}).first;
            }

            auto itO = itX->second.find(orientation);
            if(itO == itX->second.end())
            {
                itO = itX->second.insert({orientation, index}).first;
            }
            itO->second = index;
        }
    }
    else if (orientation == zyPlane)
    {
        for (int idx : edge->indices)
        {
            auto itZ = concaveEdgeMap.find(z);
            if (itZ == concaveEdgeMap.end())
            {
                // z doesn't exist, so create an entry for it
                itZ = concaveEdgeMap.insert({z, {}}).first;
            }

            auto itY = itZ->second.find(idx);
            if (itY == itZ->second.end())
            {
                // y doesn't exist, so create an entry for it
                itY = itZ->second.insert({idx, {}}).first;
            }

            auto itX = itY->second.find(x);
            if (itX == itY->second.end())
            {
                // idx doesn't exist, so create an entry for it
                itX = itY->second.insert({x, {}}).first;
            }

            // At this point, we've ensured that all outer maps exist, so we can safely use operator[] for the innermost map
            auto itO = itX->second.find(orientation);
            if(itO == itX->second.end())
            {
                itO = itX->second.insert({orientation, index}).first;
            }
            itO->second = index;
        }
    }
    else if (orientation == zxPlane)
    {
        for (int idx : edge->indices)
        {;
            auto itZ = concaveEdgeMap.find(idx);
            if (itZ == concaveEdgeMap.end())
            {
                // z doesn't exist, so create an entry for it
                itZ = concaveEdgeMap.insert({idx, {}}).first;
            }

            auto itY = itZ->second.find(y);
            if (itY == itZ->second.end())
            {
                // y doesn't exist, so create an entry for it
                itY = itZ->second.insert({y, {}}).first;
            }

            auto itX = itY->second.find(x);
            if (itX == itY->second.end())
            {
                // idx doesn't exist, so create an entry for it
                itX = itY->second.insert({x, {}}).first;
            }

            // At this point, we've ensured that all outer maps exist, so we can safely use operator[] for the innermost map
            auto itO = itX->second.find(orientation);
            if(itO == itX->second.end())
            {
                itO = itX->second.insert({orientation, index}).first;
            }
            itO->second = index;
        }
    }

}

int Decomp3D::exist_in_map(int z, int y, int x, int orientation, int_4d& concaveEdgeMap)
{
    auto it1 = concaveEdgeMap.find(z);
    if (it1 != concaveEdgeMap.end())
    {
        auto it2 = it1->second.find(y);
        if (it2 != it1->second.end())
        {
            auto it3 = it2->second.find(x);
            if (it3 != it2->second.end())
            {
                auto it4 = it3->second.find(orientation);
                if (it4 != it3->second.end())
                {
                    return it4->second;
                }
            }
        }
    }
    return -1;
}

std::vector<int> Decomp3D::calWeight(std::vector<Separator *> &separators, int_4d& concaveEdgeMap)
{
    // loop through all indicies to find which index is a concave edge using the edge map
    // then loop through the edge that is perpendicular to the separator to check for concave edge
    // the algorithm is: w(s) = number of concave edges on the separator - number of concave edges perpendicular to the separator

    // have a map to make sure that we dont count the edges already counted
    std::vector<int> weight;
    int sep_idx = 0;
    for (Separator *separator : separators)
    {
        std::unordered_map<int, bool> countedEdges;
        int concaveEdgeNumber = 0;
        int perpendicularEdgeNumber = 0;

        if (separator->orientation == xyPlane)
        {
            for (std::vector<int> &indicie : separator->indices)
            {
                // use the orientation to determine the edges.
                // we are storing a block here, so a total of 12 edges
                // should be checked(8 perpendicular, 4 edges on)

                // a more effcient way would be checking for empty blocks around the separator to identify edges
                // worth checking
                int curZ = indicie[0];
                int curY = indicie[1];
                int curX = indicie[2];

                // loop through edges in the 2 directions of the plane
                // x direction
                int index = exist_in_map(curZ + 1, curY + 1, curX, xyPlane, concaveEdgeMap);
                if (index != -1 && countedEdges[index] == false)
                {
                    countedEdges[index] = true;
                    concaveEdgeNumber++;
                }
                index = exist_in_map(curZ + 1, curY, curX, xyPlane, concaveEdgeMap);
                if (index != -1 && countedEdges[index] == false)
                {
                    countedEdges[index] = true;
                    concaveEdgeNumber++;
                }
                index = exist_in_map(curZ + 1, curY, curX + 1, zyPlane, concaveEdgeMap);
                if (index != -1 && countedEdges[index] == false)
                {
                    countedEdges[index] = true;
                    concaveEdgeNumber++;
                }
                index = exist_in_map(curZ + 1, curY, curX, zyPlane, concaveEdgeMap);
                if (index != -1 && countedEdges[index] == false)
                {
                    countedEdges[index] = true;
                    concaveEdgeNumber++;
                }

                // z direction for perpendicular edge
                index = exist_in_map(curZ + 1, curY, curX, zxPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    int index2 = exist_in_map(curZ, curY, curX, zxPlane, concaveEdgeMap);
                    if (index == index2)
                    {
                        countedEdges[index] = true;
                        perpendicularEdgeNumber++;
                    }
                }
                index = exist_in_map(curZ + 1, curY, curX + 1, zxPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    int index2 = exist_in_map(curZ, curY, curX + 1, zxPlane, concaveEdgeMap);
                    if (index == index2)
                    {
                        countedEdges[index] = true;
                        perpendicularEdgeNumber++;
                    }
                }
                index = exist_in_map(curZ + 1, curY + 1, curX, zxPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    int index2 = exist_in_map(curZ, curY + 1, curX, zxPlane, concaveEdgeMap);
                    if (index == index2)
                    {
                        countedEdges[index] = true;
                        perpendicularEdgeNumber++;
                    }
                }
                index = exist_in_map(curZ + 1, curY + 1, curX + 1, zxPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    int index2 = exist_in_map(curZ, curY + 1, curX + 1, zxPlane, concaveEdgeMap);
                    if (index == index2)
                    {
                        countedEdges[index] = true;
                        perpendicularEdgeNumber++;
                    }
                }
            }
        }

        else if (separator->orientation == zyPlane)
        {

            for (std::vector<int> &indicie : separator->indices)
            {
                // use the orientation to determine the edges.
                // we are storing a block here, so a total of 12 edges
                // should be checked(8 perpendicular, 4 edges on)

                // a more effcient way would be checking for empty blocks around the separator to identify edges
                // worth checking
                int curZ = indicie[0];
                int curY = indicie[1];
                int curX = indicie[2];
                // loop through edges in the 2 directions of the plane
                // x direction
                // 0, 0, 0 | 1, 0, 0 | 0, 1, 0 | 2, 0, 0 | 1, 1, 0 | 0, 2, 0 | 2, 1, 0 | 1, 2, 0 | 2, 2, 0 |
                int index = exist_in_map(curZ + 1, curY, curX + 1, zyPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    countedEdges[index] = true;
                    concaveEdgeNumber++;
                }
                index = exist_in_map(curZ, curY, curX + 1, zyPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    countedEdges[index] = true;
                    concaveEdgeNumber++;
                }
                index = exist_in_map(curZ, curY + 1, curX + 1, zxPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    countedEdges[index] = true;
                    concaveEdgeNumber++;
                }
                index = exist_in_map(curZ, curY, curX + 1, zxPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    countedEdges[index] = true;
                    concaveEdgeNumber++;
                }

                // x direction for perpendicular edge
                index = exist_in_map(curZ, curY, curX + 1, xyPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    int index2 = exist_in_map(curZ, curY, curX, xyPlane, concaveEdgeMap);
                    if (index == index2)
                    {
                        countedEdges[index] = true;
                        perpendicularEdgeNumber++;
                    }
                }

                index = exist_in_map(curZ + 1, curY, curX + 1, xyPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    int index2 = exist_in_map(curZ + 1, curY, curX, xyPlane, concaveEdgeMap);
                    if (index == index2)
                    {
                        countedEdges[index] = true;
                        perpendicularEdgeNumber++;
                    }
                }

                index = exist_in_map(curZ, curY + 1, curX + 1, xyPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    int index2 = exist_in_map(curZ, curY + 1, curX, xyPlane, concaveEdgeMap);
                    if (index == index2)
                    {
                        countedEdges[index] = true;
                        perpendicularEdgeNumber++;
                    }
                }
                index = exist_in_map(curZ + 1, curY + 1, curX + 1, xyPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    int index2 = exist_in_map(curZ + 1, curY + 1, curX, xyPlane, concaveEdgeMap);
                    if (index == index2)
                    {
                        countedEdges[index] = true;
                        perpendicularEdgeNumber++;
                    }
                }
            }
        }

        else if (separator->orientation == zxPlane)
        {
            for (std::vector<int> &indicie : separator->indices)
            {
                // use the orientation to determine the edges.
                // we are storing a block here, so a total of 12 edges
                // should be checked(8 perpendicular, 4 edges on)

                // a more effcient way would be checking for empty blocks around the separator to identify edges
                // worth checking
                int curZ = indicie[0];
                int curY = indicie[1];
                int curX = indicie[2];

                // loop through edges in the 2 directions of the plane
                // x direction
                int index = exist_in_map(curZ + 1, curY + 1, curX, xyPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    countedEdges[index] = true;
                    concaveEdgeNumber++;
                }
                index = exist_in_map(curZ, curY + 1, curX, xyPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    countedEdges[index] = true;
                    concaveEdgeNumber++;
                }
                index = exist_in_map(curZ, curY + 1, curX + 1, zxPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    countedEdges[index] = true;
                    concaveEdgeNumber++;
                }
                index = exist_in_map(curZ, curY + 1, curX, zxPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    countedEdges[index] = true;
                    concaveEdgeNumber++;
                }

                // y direction for perpendicular edge
                index = exist_in_map(curZ, curY + 1, curX, zyPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    int index2 = exist_in_map(curZ, curY, curX, zyPlane, concaveEdgeMap);
                    if (index == index2)
                    {

                        countedEdges[index] = true;
                        perpendicularEdgeNumber++;
                    }
                }

                index = exist_in_map(curZ, curY + 1, curX + 1, zyPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    int index2 = exist_in_map(curZ, curY, curX + 1, zyPlane, concaveEdgeMap);
                    if (index == index2)
                    {

                        countedEdges[index] = true;
                        perpendicularEdgeNumber++;
                    }
                }
                index = exist_in_map(curZ + 1, curY + 1, curX, zyPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    int index2 = exist_in_map(curZ + 1, curY, curX, zyPlane, concaveEdgeMap);
                    if (index == index2)
                    {
                        countedEdges[index] = true;
                        perpendicularEdgeNumber++;
                    }
                }
                index = exist_in_map(curZ + 1, curY + 1, curX + 1, zyPlane, concaveEdgeMap);
                if (index != -1 && countedEdges.find(index) == countedEdges.end())
                {
                    int index2 = exist_in_map(curZ + 1, curY, curX + 1, zyPlane, concaveEdgeMap);
                    if (index == index2)
                    {
                        countedEdges[index] = true;
                        perpendicularEdgeNumber++;
                    }
                }
            }
        }
        if (concaveEdgeNumber == 0)
        {
            weight.push_back(INT_MIN);
            continue;
        }
        sep_idx++;
        weight.push_back(concaveEdgeNumber - perpendicularEdgeNumber);
    }

    return weight;
}

void Decomp3D::removeUnusedSeparators(std::vector<int> &weight, std::vector<Separator *> &separators)
{
    int deleteCount = 0;
    for (unsigned int i = 0; i < separators.size(); i++)
    {
        if (weight[i] == INT_MIN)
        {
            deleteCount++;
            delete separators[i];
            continue;
        }

        weight[i - deleteCount] = weight[i];
        separators[i - deleteCount] = separators[i];
    }

    while (deleteCount--)
    {
        weight.pop_back();
        separators.pop_back();
    }
}

bool Decomp3D::ifIntersect(Separator *current, Separator *comparsion)
{
    // needed to code for the three dimensions
    // xy
    std::unordered_map<int, int> intersection_coordinates;
    if (current->orientation == xyPlane && comparsion->orientation == zyPlane)
    {
        int comp_x = comparsion->indices[0][2];
        int comp_z = current->indices[0][0];
        // extract coordinates from xyPlane
        // we know that the x is always the same so we dont need to keep track of this.
        // because for a singe z coordinate, we need the top, bottom, to belong to the current
        // left and right block to belong to the right, so therefore if
        // there is a point of intersection, the total weight at the z coordinate should be 0

        for (std::vector<int> &coordinates : current->indices)
        {
            int x = coordinates[2];
            int y = coordinates[1];
            if (x == comp_x)
            {
                intersection_coordinates[y]++;
            }
            else if (x == comp_x + 1)
            {
                intersection_coordinates[y]++;
            }
        }

        // now we check the compared separator
        for (std::vector<int> &coordinates : comparsion->indices)
        {
            int y = coordinates[1];
            int z = coordinates[0];
            if (z == comp_z)
            {
                intersection_coordinates[y]--;
            }
            else if (z == comp_z + 1)
            {
                intersection_coordinates[y]--;
            }
        }

        for(auto it = intersection_coordinates.begin(); it != intersection_coordinates.end(); it++){
            auto weight = it->second;            if (weight == 0)
                return true;
        }
    }
    else if (current->orientation == xyPlane && comparsion->orientation == zxPlane)
    {
        int comp_y = comparsion->indices[0][1];
        int comp_z = current->indices[0][0];
        // extract coordinates from xyPlane
        // we know that the x is always the same so we dont need to keep track of this.
        // because for a singe z coordinate, we need the top, bottom, to belong to the current
        // left and right block to belong to the right, so therefore if
        // there is a point of intersection, the total weight at the z coordinate should be 0

        for (std::vector<int> &coordinates : current->indices)
        {
            int y = coordinates[1];
            int x = coordinates[2];
            if (y == comp_y)
            {
                intersection_coordinates[x]++;
            }
            else if (y == comp_y + 1)
            {
                intersection_coordinates[x]++;
            }
        }
        // now we check the compared separator
        for (std::vector<int> &coordinates : comparsion->indices)
        {
            int x = coordinates[2];
            int z = coordinates[0];
            if (z == comp_z)
            {
                intersection_coordinates[x]--;
            }
            else if (z == comp_z + 1)
            {
                intersection_coordinates[x]--;
            }
        }

        for(auto it = intersection_coordinates.begin(); it != intersection_coordinates.end(); it++){
            int weight = it->second;
            if (weight == 0)
                return true;
        }
    }

    // zy
    else if (current->orientation == zyPlane && comparsion->orientation == xyPlane)
    {
        int comp_z = comparsion->indices[0][0];
        int comp_x = current->indices[0][2];
        // extract coordinates from xyPlane
        // we know that the x is always the same so we dont need to keep track of this.
        // because for a singe z coordinate, we need the top, bottom, to belong to the current
        // left and right block to belong to the right, so therefore if
        // there is a point of intersection, the total weight at the z coordinate should be 0

        for (std::vector<int> &coordinates : current->indices)
        {
            int y = coordinates[1];
            int z = coordinates[0];
            if (z == comp_z)
            {
                intersection_coordinates[y]++;
            }
            else if (z == comp_z + 1)
            {
                intersection_coordinates[y]++;
            }
        }

        // now we check the compared separator
        for (std::vector<int> &coordinates : comparsion->indices)
        {
            int y = coordinates[1];
            int x = coordinates[2];
            if (x == comp_x)
            {
                intersection_coordinates[y]--;
            }
            else if (x == comp_x + 1)
            {
                intersection_coordinates[y]--;
            }
        }

        for(auto it = intersection_coordinates.begin(); it != intersection_coordinates.end(); it++){
            auto weight = it->second;            if (weight == 0)
                return true;
        }
    }
    else if (current->orientation == zyPlane && comparsion->orientation == zxPlane)
    {
        int comp_y = comparsion->indices[0][1];
        int comp_x = current->indices[0][2];
        // extract coordinates from xyPlane
        // we know that the x is always the same so we dont need to keep track of this.
        // because for a singe z coordinate, we need the top, bottom, to belong to the current
        // left and right block to belong to the right, so therefore if
        // there is a point of intersection, the total weight at the z coordinate should be 0

        for (std::vector<int> &coordinates : current->indices)
        {
            int y = coordinates[1];
            int z = coordinates[0];
            if (y == comp_y)
            {
                intersection_coordinates[z]++;
            }
            else if (y == comp_y + 1)
            {
                intersection_coordinates[z]++;
            }
        }
        // now we check the compared separator
        for (std::vector<int> &coordinates : comparsion->indices)
        {
            int z = coordinates[0];
            int x = coordinates[2];
            if (x == comp_x)
            {
                intersection_coordinates[z]--;
            }
            else if (x == comp_x + 1)
            {
                intersection_coordinates[z]--;
            }
        }

        for(auto it = intersection_coordinates.begin(); it != intersection_coordinates.end(); it++){
            auto weight = it->second;            if (weight == 0)
            {
                return true;
            }
        }
    }
    // zx
    else if (current->orientation == zxPlane && comparsion->orientation == xyPlane)
    {
        int comp_z = comparsion->indices[0][2];
        int comp_y = current->indices[0][1];
        // extract coordinates from xyPlane
        // we know that the x is always the same so we dont need to keep track of this.
        // because for a singe z coordinate, we need the top, bottom, to belong to the current
        // left and right block to belong to the right, so therefore if
        // there is a point of intersection, the total weight at the z coordinate should be 0

        for (std::vector<int> &coordinates : current->indices)
        {
            int x = coordinates[2];
            int z = coordinates[0];
            if (z == comp_z)
            {
                intersection_coordinates[x]++;
            }
            else if (z == comp_z + 1)
            {
                intersection_coordinates[x]++;
            }
        }

        // now we check the compared separator
        for (std::vector<int> &coordinates : comparsion->indices)
        {
            int y = coordinates[1];
            int x = coordinates[2];
            if (y == comp_y)
            {
                intersection_coordinates[x]--;
            }
            else if (y == comp_y + 1)
            {
                intersection_coordinates[x]--;
            }
        }

        for(auto it = intersection_coordinates.begin(); it != intersection_coordinates.end(); it++){
            int weight = it->second;
            if (weight == 0)
                return true;
        }
    }
    else if (current->orientation == zxPlane && comparsion->orientation == zyPlane)
    {
        int comp_x = comparsion->indices[0][2];
        int comp_y = current->indices[0][1];
        // extract coordinates from xyPlane
        // we know that the x is always the same so we dont need to keep track of this.
        // because for a singe z coordinate, we need the top, bottom, to belong to the current
        // left and right block to belong to the right, so therefore if
        // there is a point of intersection, the total weight at the z coordinate should be 0

        for (std::vector<int> &coordinates : current->indices)
        {
            int x = coordinates[2];
            int z = coordinates[0];
            if (x == comp_x)
            {
                intersection_coordinates[z]++;
            }
            else if (x == comp_x + 1)
            {
                intersection_coordinates[z]++;
            }
        }

        // now we check the compared separator
        for (std::vector<int> &coordinates : comparsion->indices)
        {
            int y = coordinates[1];
            int z = coordinates[0];
            if (y == comp_y)
            {
                intersection_coordinates[z]--;
            }
            else if (y == comp_y + 1)
            {
                intersection_coordinates[z]--;
            }
        }

        for(auto it = intersection_coordinates.begin(); it != intersection_coordinates.end(); it++){
            auto weight = it->second;            if (weight == 0)
                return true;
        }
    }

    return false;
}

// creating the graph
std::vector<std::vector<int>> Decomp3D::createGraph(std::vector<Separator *> &separators)
{
    std::vector<std::vector<int>> graph(separators.size());

    for (unsigned int i = 0; i < separators.size(); i++)
    {
        for (unsigned int j = i + 1; j < separators.size(); j++)
        {
            if (separators[i]->orientation != separators[j]->orientation && ifIntersect(separators[i], separators[j]))
            {
                graph[i].push_back(j);
                graph[j].push_back(i);
            }
        }
    }

    return graph;
}

std::vector<int> Decomp3D::maxIS(std::vector<std::vector<int>> &graph)
{
    std::vector<std::vector<int>> degree;
    for (int i = 0; i < (int)graph.size(); i++)
    {
        degree.push_back({(int)graph[i].size(), i});
    }

    std::sort(degree.begin(), degree.end(),

              [](std::vector<int> &a, std::vector<int> &b)
              {
                  return a[0] < b[0];
              });

    std::vector<int> set;
    for (unsigned int i = 0; i < degree.size(); i++)
    {

        bool inSet = false;
        // check if there is a neighbor in the set
        for (int index : set)
        {

            for (int neighbor : graph[degree[i][1]])
            {
                if (neighbor == index)
                {
                    inSet = true;
                    break;
                }
            }
            if (inSet)
                break;
        }

        if (!inSet)
            set.push_back(degree[i][1]);
    }

    return set;
}

void Decomp3D::erase_mapEdges(CEdge *edge, int_4d& concaveEdgeMap)
{

    int orientation = edge->orientation;
    int z = edge->zStart;
    int y = edge->yStart;
    int x = edge->xStart;
    if (orientation == xyPlane)
    {
        for (int idx : edge->indices)
        {

            auto it = concaveEdgeMap.find(z);
            auto it2 = it->second.find(y);
            auto it3 = it2->second.find(idx);
            auto it4 = it3->second.find(orientation);
            it3->second.erase(it4);
            if (it3->second.empty())
            {
                it2->second.erase(it3);
                if (it2->second.empty())
                {
                    it->second.erase(it2);
                    if(it->second.empty()){
                        concaveEdgeMap.erase(it);
                    }
                }
            }
        }
    }
    else if (orientation == zyPlane)
    {
        for (int idx : edge->indices)
        {
            auto it = concaveEdgeMap.find(z);
            auto it2 = it->second.find(idx);
            auto it3 = it2->second.find(x);
            auto it4 = it3->second.find(orientation);
            it3->second.erase(it4);
            if (it3->second.empty())
            {
                it2->second.erase(it3);
                if (it2->second.empty())
                {
                    it->second.erase(it2);
                    if(it->second.empty()){
                        concaveEdgeMap.erase(it);
                    }
                }
            }
        }
    }
    else if (orientation == zxPlane)
    {
        for (int idx : edge->indices)
        {

            auto it = concaveEdgeMap.find(idx);
            auto it2 = it->second.find(y);
            auto it3 = it2->second.find(x);
            auto it4 = it3->second.find(orientation);
            it3->second.erase(it4);
            if (it3->second.empty())
            {
                it2->second.erase(it3);
                if (it2->second.empty())
                {
                    it->second.erase(it2);
                    if(it->second.empty()){
                        concaveEdgeMap.erase(it);
                    }
                }
            }
        }
    }
}

// remove edges for separators placed as walls
void Decomp3D::removeEdges(std::vector<CEdge *> &edges, std::vector<Separator *> &separators, int_4d& concaveEdgeMap)
{
    // loop through all indicies to find which index is a concave edge using the edge map

    for (Separator *separator : separators)
    {

        if (separator->orientation == xyPlane)
        {
            for (std::vector<int> &indicie : separator->indices)
            {
                // use the orientation to determine the edges.
                // we are storing a block here, so a total of 12 edges
                // should be checked(8 perpendicular, 4 edges on)

                // a more effcient way would be checking for empty blocks around the separator to identify edges
                // worth checking
                int curZ = indicie[0];
                int curY = indicie[1];
                int curX = indicie[2];

                // loop through edges in the 2 directions of the plane
                // x direction
                int index = exist_in_map(curZ + 1, curY + 1, curX, xyPlane, concaveEdgeMap);
                if (index != -1)
                {
                    // std::cout << "ERASING INDEX: " << index << std::endl;
                    erase_mapEdges(edges.at(index), concaveEdgeMap);
                    delete edges[index];
                    edges[index] = nullptr;
                }

                index = exist_in_map(curZ + 1, curY, curX, xyPlane, concaveEdgeMap);
                if (index != -1)
                {
                    // std::cout << "ERASING INDEX: " << index << std::endl;
                    erase_mapEdges(edges.at(index), concaveEdgeMap);
                    delete edges[index];
                    edges[index] = nullptr;
                }

                index = exist_in_map(curZ + 1, curY, curX + 1, zyPlane, concaveEdgeMap);
                if (index != -1)
                {
                    // std::cout << "ERASING INDEX: " << index << std::endl;
                    erase_mapEdges(edges.at(index), concaveEdgeMap);
                    delete edges[index];
                    edges[index] = nullptr;
                }

                index = exist_in_map(curZ + 1, curY, curX, zyPlane, concaveEdgeMap);
                if (index != -1)
                {
                    // std::cout << "ERASING INDEX: " << index << std::endl;
                    erase_mapEdges(edges.at(index), concaveEdgeMap);
                    delete edges[index];
                    edges[index] = nullptr;
                }
            }
        }
        else if (separator->orientation == zyPlane)
        {

            for (std::vector<int> &indicie : separator->indices)
            {
                // use the orientation to determine the edges.
                // we are storing a block here, so a total of 12 edges
                // should be checked(8 perpendicular, 4 edges on)

                // a more effcient way would be checking for empty blocks around the separator to identify edges
                // worth checking
                int curZ = indicie[0];
                int curY = indicie[1];
                int curX = indicie[2];

                // loop through edges in the 2 directions of the plane
                // x direction
                int index = exist_in_map(curZ + 1, curY, curX + 1, zyPlane, concaveEdgeMap);
                if (index != -1)
                {
                    erase_mapEdges(edges.at(index), concaveEdgeMap);
                    delete edges[index];
                    edges[index] = nullptr;
                }
                index = exist_in_map(curZ, curY, curX + 1, zyPlane, concaveEdgeMap);
                if (index != -1)
                {

                    erase_mapEdges(edges.at(index), concaveEdgeMap);
                    delete edges[index];
                    edges[index] = nullptr;
                }
                index = exist_in_map(curZ, curY + 1, curX + 1, zxPlane, concaveEdgeMap);
                if (index != -1)
                {

                    erase_mapEdges(edges.at(index), concaveEdgeMap);
                    delete edges[index];
                    edges[index] = nullptr;
                }
                index = exist_in_map(curZ, curY, curX + 1, zxPlane, concaveEdgeMap);
                if (index != -1)
                {
                    erase_mapEdges(edges.at(index), concaveEdgeMap);
                    delete edges[index];
                    edges[index] = nullptr;
                }
            }
        }
        else if (separator->orientation == zxPlane)
        {
            for (std::vector<int> &indicie : separator->indices)
            {
                // use the orientation to determine the edges.
                // we are storing a block here, so a total of 12 edges
                // should be checked(8 perpendicular, 4 edges on)

                // a more effcient way would be checking for empty blocks around the separator to identify edges
                // worth checking
                int curZ = indicie[0];
                int curY = indicie[1];
                int curX = indicie[2];

                // loop through edges in the 2 directions of the plane
                // x direction
                int index = exist_in_map(curZ + 1, curY + 1, curX, xyPlane, concaveEdgeMap);
                if (index != -1)
                {
                    erase_mapEdges(edges.at(index), concaveEdgeMap);
                    delete edges[index];
                    edges[index] = nullptr;
                }
                index = exist_in_map(curZ, curY + 1, curX, xyPlane, concaveEdgeMap);
                if (index != -1)
                {
                    erase_mapEdges(edges.at(index), concaveEdgeMap);
                    delete edges[index];
                    edges[index] = nullptr;
                }
                index = exist_in_map(curZ, curY + 1, curX + 1, zxPlane, concaveEdgeMap);
                if (index != -1)
                {
                    erase_mapEdges(edges.at(index), concaveEdgeMap);
                    delete edges[index];
                    edges[index] = nullptr;
                }
                index = exist_in_map(curZ, curY + 1, curX, zxPlane, concaveEdgeMap);
                if (index != -1)
                {
                    erase_mapEdges(edges.at(index), concaveEdgeMap);
                    delete edges[index];
                    edges[index] = nullptr;
                }
            }
        }
    }

    int deleteCount = 0;

    for (unsigned int i = 0; i < edges.size(); i++)
    {
        
        if (edges[i] == nullptr)
        {
            deleteCount++;
            continue;
        }

        // std::cout << "MOVING " << i << " | to | " << i - deleteCount << std::endl;
        // updating map entries
        erase_mapEdges(edges[i], concaveEdgeMap);
        insert_to_map(edges[i], i-deleteCount, concaveEdgeMap);
        edges[i - deleteCount] = edges[i];
    }

    while (deleteCount--)
        edges.pop_back();
}

std::vector<Separator *> Decomp3D::setSeparator(std::vector<Separator *> &separators, std::vector<int> &indices)
{
    std::vector<Separator *> ret;
    for (int idx : indices)
    {
        ret.push_back(separators[idx]);
    }

    return ret;
}

void Decomp3D::removeSeparatorList(std::vector<Separator *> &separators, std::vector<Separator *> &finalSeparator)
{
    // std::cout << "I CONTAINS: " << separators.size() << std::endl;
    // std::cout << "J CONTAINSL " << finalSeparator.size() << std::endl;
    for (size_t j = 0; j < finalSeparator.size(); j++)
    {
        // std::cout << "j: " << j << std::endl;
        for (size_t i = 0; i < separators.size(); i++)
        {
            // std::cout << "i: " << i << std::endl;
            if (finalSeparator[j] == separators[i])
            {
                separators[i] = nullptr;
                break;
            }
        }
    }

    int deleteCount = 0;

    for (size_t i = 0; i < separators.size(); i++)
    {
        // std::cout << "i2: " << i << std::endl;
        if (separators[i] == nullptr)
        {
            deleteCount++;
            continue;
        }
        separators[i - deleteCount] = separators[i];
    }

    while (deleteCount--)
        separators.pop_back();
}

bool Decomp3D::ifIntersect_splitting(Separator *current, Separator *comparsion)
{
    // needed to code for the three dimensions
    // xy

    std::unordered_map<int, std::pair<int, int>> intersection_coordinates;
    if (current->orientation == xyPlane && comparsion->orientation == zyPlane)
    {
        int comp_x = comparsion->indices[0][2];
        int comp_z = current->indices[0][0];
        // extract coordinates from xyPlane
        // we know that the x is always the same so we dont need to keep track of this.
        // because for a singe z coordinate, we need the top, bottom, to belong to the current
        // left and right block to belong to the right, so therefore if
        // there is a point of intersection, the total weight at the z coordinate should be 0

        for (std::vector<int> &coordinates : current->indices)
        {
            int x = coordinates[2];
            int y = coordinates[1];
            if (x == comp_x)
            {
                intersection_coordinates[y].first++;
            }
            else if (x == comp_x + 1)
            {
                intersection_coordinates[y].first++;
            }
        }

        // now we check the compared separator
        for (std::vector<int> &coordinates : comparsion->indices)
        {
            int y = coordinates[1];
            int z = coordinates[0];
            if (z == comp_z)
            {
                intersection_coordinates[y].second--;
            }
            else if (z == comp_z + 1)
            {
                intersection_coordinates[y].second--;
            }
        }

        for(auto it = intersection_coordinates.begin(); it != intersection_coordinates.end(); it++){
            auto weight = it->second;            if (weight.first == 2 && weight.second == -2)
                return true;
        }
    }
    else if (current->orientation == xyPlane && comparsion->orientation == zxPlane)
    {
        int comp_y = comparsion->indices[0][1];
        int comp_z = current->indices[0][0];
        // extract coordinates from xyPlane
        // we know that the x is always the same so we dont need to keep track of this.
        // because for a singe z coordinate, we need the top, bottom, to belong to the current
        // left and right block to belong to the right, so therefore if
        // there is a point of intersection, the total weight at the z coordinate should be 0

        for (std::vector<int> &coordinates : current->indices)
        {
            int y = coordinates[1];
            int x = coordinates[2];
            if (y == comp_y)
            {
                intersection_coordinates[x].first++;
            }
            else if (y == comp_y + 1)
            {
                intersection_coordinates[x].first++;
            }
        }
        // now we check the compared separator
        for (std::vector<int> &coordinates : comparsion->indices)
        {
            int x = coordinates[2];
            int z = coordinates[0];
            if (z == comp_z)
            {
                intersection_coordinates[x].second--;
            }
            else if (z == comp_z + 1)
            {
                intersection_coordinates[x].second--;
            }
        }

        for(auto it = intersection_coordinates.begin(); it != intersection_coordinates.end(); it++){
            auto weight = it->second;
            if (weight.first == 2 && weight.second == -2)
                return true;
        }
    }

    // zy
    else if (current->orientation == zyPlane && comparsion->orientation == xyPlane)
    {

        int comp_z = comparsion->indices[0][0];
        int comp_x = current->indices[0][2];

        // extract coordinates from xyPlane
        // we know that the x is always the same so we dont need to keep track of this.
        // because for a singe z coordinate, we need the top, bottom, to belong to the current
        // left and right block to belong to the right, so therefore if
        // there is a point of intersection, the total weight at the z coordinate should be 0

        for (std::vector<int> &coordinates : current->indices)
        {
            int y = coordinates[1];
            int z = coordinates[0];
            if (z == comp_z)
            {
                intersection_coordinates[y].first++;
            }
            else if (z == comp_z + 1)
            {
                intersection_coordinates[y].first++;
            }
        }

        // now we check the compared separator
        for (std::vector<int> &coordinates : comparsion->indices)
        {
            int y = coordinates[1];
            int x = coordinates[2];
            if (x == comp_x)
            {
                intersection_coordinates[y].second--;
            }
            else if (x == comp_x + 1)
            {
                intersection_coordinates[y].second--;
            }
        }


        for(auto it = intersection_coordinates.begin(); it != intersection_coordinates.end(); it++){
            auto weight = it->second;
            if (weight.first == 2 && weight.second == -2)
            {

                return true;
            }
        }
    }
    else if (current->orientation == zyPlane && comparsion->orientation == zxPlane)
    {
        int comp_y = comparsion->indices[0][1];
        int comp_x = current->indices[0][2];
        // extract coordinates from xyPlane
        // we know that the x is always the same so we dont need to keep track of this.
        // because for a singe z coordinate, we need the top, bottom, to belong to the current
        // left and right block to belong to the right, so therefore if
        // there is a point of intersection, the total weight at the z coordinate should be 0

        for (std::vector<int> &coordinates : current->indices)
        {
            int y = coordinates[1];
            int z = coordinates[0];
            if (y == comp_y)
            {
                intersection_coordinates[z].first++;
            }
            else if (y == comp_y + 1)
            {
                intersection_coordinates[z].first++;
            }
        }
        // now we check the compared separator
        for (std::vector<int> &coordinates : comparsion->indices)
        {
            int z = coordinates[0];
            int x = coordinates[2];
            if (x == comp_x)
            {
                intersection_coordinates[z].second--;
            }
            else if (x == comp_x + 1)
            {
                intersection_coordinates[z].second--;
            }
        }

        for(auto it = intersection_coordinates.begin(); it != intersection_coordinates.end(); it++){
            auto weight = it->second;            if (weight.first == 2 && weight.second == -2)
            {
                return true;
            }
        }
    }
    // zx
    else if (current->orientation == zxPlane && comparsion->orientation == xyPlane)
    {
        int comp_z = comparsion->indices[0][2];
        int comp_y = current->indices[0][1];
        // extract coordinates from xyPlane
        // we know that the x is always the same so we dont need to keep track of this.
        // because for a singe z coordinate, we need the top, bottom, to belong to the current
        // left and right block to belong to the right, so therefore if
        // there is a point of intersection, the total weight at the z coordinate should be 0

        for (std::vector<int> &coordinates : current->indices)
        {
            int x = coordinates[2];
            int z = coordinates[0];
            if (z == comp_z)
            {
                intersection_coordinates[x].first++;
            }
            else if (z == comp_z + 1)
            {
                intersection_coordinates[x].first++;
            }
        }

        // now we check the compared separator
        for (std::vector<int> &coordinates : comparsion->indices)
        {
            int y = coordinates[1];
            int x = coordinates[2];
            if (y == comp_y)
            {
                intersection_coordinates[x].second--;
            }
            else if (y == comp_y + 1)
            {
                intersection_coordinates[x].second--;
            }
        }

        for(auto it = intersection_coordinates.begin(); it != intersection_coordinates.end(); it++){
            auto weight = it->second;
            if (weight.first == 2 && weight.second == -2)
                return true;
        }
    }
    else if (current->orientation == zxPlane && comparsion->orientation == zyPlane)
    {
        int comp_x = comparsion->indices[0][2];
        int comp_y = current->indices[0][1];
        // extract coordinates from xyPlane
        // we know that the x is always the same so we dont need to keep track of this.
        // because for a singe z coordinate, we need the top, bottom, to belong to the current
        // left and right block to belong to the right, so therefore if
        // there is a point of intersection, the total weight at the z coordinate should be 0

        for (std::vector<int> &coordinates : current->indices)
        {
            int x = coordinates[2];
            int z = coordinates[0];
            if (x == comp_x)
            {
                intersection_coordinates[z].first++;
            }
            else if (x == comp_x + 1)
            {
                intersection_coordinates[z].first++;
            }
        }

        // now we check the compared separator
        for (std::vector<int> &coordinates : comparsion->indices)
        {
            int y = coordinates[1];
            int z = coordinates[0];
            if (y == comp_y)
            {
                intersection_coordinates[z].second--;
            }
            else if (y == comp_y + 1)
            {
                intersection_coordinates[z].second--;
            }
        }

        for(auto it = intersection_coordinates.begin(); it != intersection_coordinates.end(); it++){
            auto weight = it->second;            if (weight.first == 2 && weight.second == -2)
                return true;
        }
    }

    return false;
}

std::vector<Separator *> Decomp3D::splitSeparators(std::vector<Separator *> &separators, std::vector<Separator *> &finalSeparator)
{
    int separatorSize = separators.size();
    std::vector<Separator *> newSep;

    for (int i = 0; i < separatorSize; i++)
    {
        std::vector<int> xAxis;
        std::vector<int> yAxis;
        std::vector<int> zAxis;
        std::vector<std::vector<Separator *>> newSeparators;

        for (unsigned int j = 0; j < finalSeparator.size(); j++)
        {

            if (separators[i]->orientation != finalSeparator[j]->orientation)
            {
                if (ifIntersect_splitting(separators[i], finalSeparator[j]) == false)
                    continue;

                // split the separator by the axis of the separators
                if (finalSeparator[j]->orientation == xyPlane)
                {
                    zAxis.push_back(finalSeparator[j]->indices[0][0] + 1);
                }
                else if (finalSeparator[j]->orientation == zxPlane)
                {
                    yAxis.push_back(finalSeparator[j]->indices[0][1] + 1);
                }
                else if (finalSeparator[j]->orientation == zyPlane)
                {
                    xAxis.push_back(finalSeparator[j]->indices[0][2] + 1);
                }
            }
        }

        // we should only split if there is at least one intersection.
        if (zAxis.empty() && yAxis.empty() && xAxis.empty())
            continue;

        // split the separators with the axis.
        if (separators[i]->orientation == xyPlane)
        {

            // bfs through the indicies, handle the axis as walls
            newSeparators.resize(xAxis.size() + 1, std::vector<Separator *>(yAxis.size() + 1, nullptr));

            std::sort(xAxis.begin(), xAxis.end());
            std::sort(yAxis.begin(), yAxis.end());

            for (std::vector<int> &indice : separators[i]->indices)
            {
                // upperbound on the x and y axis?
                // check if the current x/y axis is within the upperbound, if not then store
                // it as yAxis index + xAxis.size()
                int curX = indice[2];
                int curY = indice[1];
                int xBound, yBound;

                xBound = std::upper_bound(xAxis.begin(), xAxis.end(), curX) - xAxis.begin();
                yBound = std::upper_bound(yAxis.begin(), yAxis.end(), curY) - yAxis.begin();

                if (newSeparators.at(xBound).at(yBound) == nullptr)
                    newSeparators[xBound][yBound] = new Separator(xyPlane);

                if (newSeparators[xBound][yBound] == nullptr)
                    newSeparators[xBound][yBound] = new Separator(xyPlane);

                newSeparators[xBound][yBound]->indices.push_back(indice);
            }
        }

        else if (separators[i]->orientation == zyPlane)
        {

            // bfs through the indicies, handle the axis as walls
            newSeparators.resize(zAxis.size() + 1, std::vector<Separator *>(yAxis.size() + 1, nullptr));

            std::sort(zAxis.begin(), zAxis.end());
            std::sort(yAxis.begin(), yAxis.end());

            for (std::vector<int> &indice : separators[i]->indices)
            {
                // upperbound on the x and y axis?
                // check if the current x/y axis is within the upperbound, if not then store
                // it as yAxis index + xAxis.size()
                int curZ = indice[0];
                int curY = indice[1];
                int zBound, yBound;

                zBound = upper_bound(zAxis.begin(), zAxis.end(), curZ) - zAxis.begin();
                yBound = upper_bound(yAxis.begin(), yAxis.end(), curY) - yAxis.begin();

                if (newSeparators[zBound][yBound] == nullptr)
                    newSeparators[zBound][yBound] = new Separator(zyPlane);

                newSeparators[zBound][yBound]->indices.push_back(indice);
            }
        }
        else if (separators[i]->orientation == zxPlane)
        {

            // bfs through the indicies, handle the axis as walls
            newSeparators.resize(xAxis.size() + 1, std::vector<Separator *>(zAxis.size() + 1, nullptr));

            std::sort(xAxis.begin(), xAxis.end());
            std::sort(zAxis.begin(), zAxis.end());

            for (std::vector<int> &indice : separators[i]->indices)
            {
                // upperbound on the x and y axis?
                // check if the current x/y axis is within the upperbound, if not then store
                // it as yAxis index + xAxis.size()
                int curX = indice[2];
                int curZ = indice[0];
                int xBound, zBound;

                xBound = std::upper_bound(xAxis.begin(), xAxis.end(), curX) - xAxis.begin();
                zBound = std::upper_bound(zAxis.begin(), zAxis.end(), curZ) - zAxis.begin();

                if (newSeparators[xBound][zBound] == nullptr)
                    newSeparators[xBound][zBound] = new Separator(zxPlane);

                newSeparators[xBound][zBound]->indices.push_back(indice);
            }
        }

        delete separators[i];
        separators[i] = newSeparators[0][0];
        newSep.push_back(newSeparators[0][0]);

        for (size_t m = 0; m < newSeparators.size(); m++)
        {
            for (size_t n = 0; n < newSeparators[m].size(); n++)
            {

                if ((m == 0 && n == 0) || newSeparators[m][n] == nullptr)
                    continue;

                newSep.push_back(newSeparators[m][n]);
                separators.push_back(newSeparators[m][n]);
            }
        }
    }
    return newSep;
}

bool Decomp3D::contain(int z, int y, int x, Separator *separator)
{

    for (std::vector<int> &indice : separator->indices)
        if (indice[0] == z && indice[1] == y && indice[2] == x)
            return true;
    return false;
}

bool Decomp3D::ifIntersect_edge(CEdge *edge, Separator *separator)
{

    int x = edge->xStart;
    int y = edge->yStart;
    int z = edge->zStart;
    if (edge->orientation == xyPlane)
    {
        // dont care about the the last since it can't be intersecting
        for (unsigned int i = 0; i < edge->indices.size() - 1; i++)
        {
            // check the 4 blocks around the edge to see if its in the separator,
            // if it this, then the edge is intersecting the separator so you
            // return true
            if (contain(z, y, edge->indices[i], separator) || contain(z - 1, y, edge->indices[i], separator) || contain(z, y - 1, edge->indices[i], separator) || contain(z - 1, y - 1, edge->indices[i], separator))
            {
                return true;
            }
        }
    }
    else if (edge->orientation == zyPlane)
    {
        // dont care about the the last since it can't be intersecting
        for (unsigned int i = 0; i < edge->indices.size() - 1; i++)
        {
            // check the 4 blocks around the edge to see if its in the separator,
            // if it this, then the edge is intersecting the separator so you
            // return true
            if (contain(z, edge->indices[i], x, separator) || contain(z - 1, edge->indices[i], x, separator) || contain(z, edge->indices[i], x - 1, separator) || contain(z - 1, edge->indices[i], x - 1, separator))
            {

                return true;
            }
        }
    }
    else if (edge->orientation == zxPlane)
    {
        // dont care about the the last since it can't be intersecting
        for (unsigned int i = 0; i < edge->indices.size() - 1; i++)
        {
            // check the 4 blocks around the edge to see if its in the separator,
            // if it this, then the edge is intersecting the separator so you
            // return true
            if (contain(edge->indices[i], y, x, separator) || contain(edge->indices[i], y, x - 1, separator) || contain(edge->indices[i], y - 1, x, separator) || contain(edge->indices[i], y - 1, x - 1, separator))
            {

                return true;
            }
        }
    }

    return false;
}

std::vector<CEdge *> Decomp3D::splitEdges(std::vector<CEdge *> &CEdges, std::vector<Separator *> &finalSeparators, int_4d& concaveEdgeMap)
{
    // the outer edges shouldn't be splitted so the code does not check for them
    // only the inner edges will be checked, and the edge should be perpendicular
    // to the separator, and there is only one orientation compared to splitting
    // separators
    int edgesSize = CEdges.size();
    std::vector<CEdge *> newEdge;
    for (int i = 0; i < edgesSize; i++)
    {
        std::vector<int> splitAxis;
        for (unsigned int j = 0; j < finalSeparators.size(); j++)
        {
            // current separator's orientation +2 mode 3 is the orientation
            // of the edge that is perpendicular to this separator
            if (CEdges[i]->orientation == (finalSeparators[j]->orientation + 2) % 3)
            {
                if (ifIntersect_edge(CEdges[i], finalSeparators[j]) == false)
                    continue;

                if (finalSeparators[j]->orientation == xyPlane)
                {
                    splitAxis.push_back(finalSeparators[j]->indices[0][0] + 1);
                }
                else if (finalSeparators[j]->orientation == zyPlane)
                {
                    splitAxis.push_back(finalSeparators[j]->indices[0][2] + 1);
                }
                else if (finalSeparators[j]->orientation == zxPlane)
                {
                    splitAxis.push_back(finalSeparators[j]->indices[0][1] + 1);
                }
            }
        }

        if (splitAxis.empty())
            continue;

        std::sort(splitAxis.begin(), splitAxis.end());

        std::vector<CEdge *> newEdges(splitAxis.size() + 1, nullptr);

        for (unsigned int k = 0; k < CEdges[i]->indices.size(); k++)
        {
            int index = std::upper_bound(splitAxis.begin(), splitAxis.end(), CEdges[i]->indices[k]) - splitAxis.begin();
            if (newEdges[index] == nullptr)
            {
                newEdges[index] = new CEdge(CEdges[i]->orientation);
                if (CEdges[i]->orientation == xyPlane)
                {
                    newEdges[index]->zStart = CEdges[i]->zStart;
                    newEdges[index]->yStart = CEdges[i]->yStart;
                    newEdges[index]->xStart = CEdges[i]->indices[k];
                }
                else if (CEdges[i]->orientation == zyPlane)
                {
                    newEdges[index]->zStart = CEdges[i]->zStart;
                    newEdges[index]->yStart = CEdges[i]->indices[k];
                    newEdges[index]->xStart = CEdges[i]->xStart;
                }
                else if (CEdges[i]->orientation == zxPlane)
                {
                    newEdges[index]->zStart = CEdges[i]->indices[k];
                    newEdges[index]->yStart = CEdges[i]->yStart;
                    newEdges[index]->xStart = CEdges[i]->xStart;
                }
            }

            newEdges[index]->indices.push_back(CEdges[i]->indices[k]);
        }

        erase_mapEdges(CEdges[i], concaveEdgeMap);

        delete CEdges[i];
        CEdges[i] = newEdges[0];
        insert_to_map(newEdges[0], i, concaveEdgeMap);

        newEdge.push_back(newEdges[0]);

        for (unsigned int k = 1; k < newEdges.size(); k++)
            if (newEdges[k] != nullptr)
            {
                insert_to_map(newEdges[k], CEdges.size(), concaveEdgeMap);
                newEdge.push_back(newEdges[k]);
                CEdges.push_back(newEdges[k]);
            }
    }

    return newEdge;
}

Block *Decomp3D::formSingleBlock(int zStart, int yStart, int xStart, int zLim, int yLim, int xLim, char blockType,
                                 std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, bool>>>> &wallMap,
                                 std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, bool>>> &hmap)
{

    int z = zStart;
    int y = yStart;
    int x = xStart;

    while (z < zLim - 1)
    {
        if ((*mySlices)[z + 1][yStart][xStart] != blockType)
            break;

        if (wallMap.find(z) != wallMap.end() && wallMap[z].find(yStart) != wallMap[z].end() && wallMap[z][yStart].find(xStart) != wallMap[z][yStart].end() && wallMap[z][yStart][xStart].find(xyPlane) != wallMap[z][yStart][xStart].end())
        {
            break;
        }

        z++;
    }

    while (y < yLim - 1)
    {
        if ((*mySlices)[zStart][y + 1][xStart] != blockType)
            break;

        if (wallMap.find(zStart) != wallMap.end() && wallMap[zStart].find(y) != wallMap[zStart].end() && wallMap[zStart][y].find(xStart) != wallMap[zStart][y].end() && wallMap[zStart][y][xStart].find(zxPlane) != wallMap[zStart][y][xStart].end())
        {
            break;
        }

        y++;
    }

    while (x < xLim - 1)
    {
        if ((*mySlices)[zStart][yStart][x + 1] != blockType)
            break;
        if (wallMap.find(zStart) != wallMap.end() && wallMap[zStart].find(yStart) != wallMap[zStart].end() && wallMap[zStart][yStart].find(x) != wallMap[zStart][yStart].end() && wallMap[zStart][yStart][x].find(zyPlane) != wallMap[zStart][yStart][x].end())
        {
            break;
        }

        x++;
    }

    for (int i = zStart; i <= z; i++)
    {
        for (int j = yStart; j <= y; j++)
        {
            for (int k = xStart; k <= x; k++)
            {
                hmap[i][j][k] = true;
            }
        }
    }

    Block *block = new Block();

    block->zStart = zStart;
    block->yStart = yStart;
    block->xStart = xStart;

    block->zDimension = z - zStart + 1;
    block->yDimension = y - yStart + 1;
    block->xDimension = x - xStart + 1;

    return block;
}

std::vector<Block *> Decomp3D::formingBlocks(int zStart, int yStart, int xStart, int zLim, int yLim, int xLim, char blockType,
                                             std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, bool>>>> &wallMap)
{

    std::vector<Block *> blocks;
    std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, bool>>> hmap;

    for (int z = zStart; z < zLim; z++)
    {
        for (int y = yStart; y < yLim; y++)
        {
            for (int x = xStart; x < xLim; x++)
            {
                // if found then continue
                if (hmap.find(z) != hmap.end() && hmap[z].find(y) != hmap[z].end() && hmap[z][y].find(x) != hmap[z][y].end())
                {
                    continue;
                }

                // or if the block type doesn't match, just continue
                if ((*mySlices)[z][y][x] != blockType)
                    continue;

                blocks.push_back(formSingleBlock(z, y, x, zLim, yLim, xLim, blockType, wallMap, hmap));
            }
        }
    }

    return blocks;
}

void Decomp3D::mergeSingle(Block *cur, std::vector<Block *> &blocks)
{

    std::vector<int> index_for_merging;
    bool merged = true;
    // z direction
    while (merged)
    {
        merged = false;
        for (unsigned int i = 0; i < blocks.size(); i++)
        {
            if (blocks[i] == nullptr)
                continue;
            if (blocks[i] == cur)
                continue;

            if (cur->xStart == blocks[i]->xStart && cur->yStart == blocks[i]->yStart && cur->xDimension == blocks[i]->xDimension && cur->yDimension == blocks[i]->yDimension)
            {
                // check if the dimension match
                if (cur->zStart - blocks[i]->zDimension == blocks[i]->zStart || cur->zStart + cur->zDimension == blocks[i]->zStart)
                {
                    // merging

                    cur->zStart = std::min(cur->zStart, blocks[i]->zStart);
                    cur->zDimension += blocks[i]->zDimension;

                    delete blocks[i];
                    blocks[i] = nullptr;

                    merged = true;
                }
            }
        }
    }

    merged = true;
    // y direction
    while (merged)
    {
        merged = false;
        for (unsigned int i = 0; i < blocks.size(); i++)
        {
            if (blocks[i] == nullptr)
                continue;
            if (blocks[i] == cur)
                continue;

            if (cur->xStart == blocks[i]->xStart && cur->zStart == blocks[i]->zStart && cur->xDimension == blocks[i]->xDimension && cur->zDimension == blocks[i]->zDimension)
            {
                // check if the dimension match
                if (cur->yStart - blocks[i]->yDimension == blocks[i]->yStart || cur->yStart + cur->yDimension == blocks[i]->yStart)
                {
                    // merging
                    cur->yStart = std::min(cur->yStart, blocks[i]->yStart);
                    cur->yDimension += blocks[i]->yDimension;

                    delete blocks[i];
                    blocks[i] = nullptr;

                    merged = true;
                }
            }
        }
    }

    merged = true;
    // x direction
    while (merged)
    {
        merged = false;
        for (unsigned int i = 0; i < blocks.size(); i++)
        {
            if (blocks[i] == nullptr)
                continue;
            if (blocks[i] == cur)
                continue;

            if (cur->yStart == blocks[i]->yStart && cur->zStart == blocks[i]->zStart && cur->yDimension == blocks[i]->yDimension && cur->zDimension == blocks[i]->zDimension)
            {
                // check if the dimension match
                if (cur->xStart - blocks[i]->xDimension == blocks[i]->xStart || cur->xStart + cur->xDimension == blocks[i]->xStart)
                {
                    // merging
                    cur->xStart = std::min(cur->xStart, blocks[i]->xStart);
                    cur->xDimension += blocks[i]->xDimension;

                    delete blocks[i];
                    blocks[i] = nullptr;

                    merged = true;
                }
            }
        }
    }
}

void Decomp3D::mergeAdjacentBlock(std::vector<Block *> &blocks)
{
    for (unsigned int i = 0; i < blocks.size(); i++)
    {
        if (blocks[i] == nullptr)
            continue;
        mergeSingle(blocks[i], blocks);
    }
}

void Decomp3D::clearVariables(std::vector<CEdge *> &concaveEdges, std::vector<Separator *> &walls)
{
    for (Separator *sep : walls)
        delete sep;

    for (CEdge *edge : concaveEdges)
        delete edge;

    walls.clear();
    concaveEdges.clear();
}