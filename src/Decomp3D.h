/**
 * @file Decomp3D.h
 * @brief Decomp3D class to encode input.
 *
 * This class provides a implementation of 3d decomposition for compression
 * It separates a single block via planes and split the slices into retangular
 * blocks. 
 */

#pragma once
#include "compression.h"

/**
 * @class Decomp3D
 * @brief Derived class for complex 3D structure compression.
 *
 * This class provides a very complex 3D decomposition method that aims for best compression 
 * rate. It uses multiple times the memory usage of a parent block. Optimisation may be required
 * for future competitions.
 */
class Decomp3D : public Compression
{
private:
    std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, int>>>> concaveEdgeMap;
    std::vector<Block*> CompressBlockType(int x_start, int y_start, int z_start, int xLim, int yLim, int zLim, char blockType);
    Separator *findSingleSeparator_xy(int x, int y, int z, int xLim, int yLim, char blockType, bool_4d &visited);
    Separator *findSingleSeparator_zy(int x, int y, int z, int yLim, int zLim, char blockType, bool_4d &visited);
    Separator *findSingleSeparator_zx(int x, int y, int z, int xLim, int zLim, char blockType, bool_4d &visited);
    std::vector<Separator *> findAllSeparators(int xStart, int yStart, int zStart, int zLim, int yLim, int xLim, char blockType);
    CEdge *findSingleConcaveEdge_xdirection(int &x, int y, int z, int xLim, char blockType);
    CEdge *findSingleConcaveEdge_ydirection(int x, int &y, int z, int yLim, char blockType);
    CEdge *findSingleConcaveEdge_zdirection(int x, int y, int &z, int zLim, char blockType);
    std::vector<CEdge *> findAllConcaveEdges(int xStart, int yStart, int zStart, int zLim, int yLim, int xLim, char blockType);
    void insert_to_map(CEdge *edge, int index);
    int exist_in_map(int z, int y, int x, int orientation);                                  
    std::vector<int> calWeight(std::vector<Separator *> &separators);
    void removeUnusedSeparators(std::vector<int> &weight, std::vector<Separator *> &separators);
    bool ifIntersect(Separator *current, Separator *comparsion);
    std::vector<std::vector<int>> createGraph(std::vector<Separator *> &separators);
    std::vector<int> maxIS(std::vector<std::vector<int>> &graph);
    void erase_mapEdges(CEdge *edge);
    std::vector<Separator *> setSeparator(std::vector<Separator *> &separators, std::vector<int> indices);
    void removeEdges(std::vector<CEdge *> &edges, std::vector<Separator *> &separators);
    void removeSeparatorList(std::vector<Separator *> &separators, std::vector<Separator *> &finalSeparator);
    bool ifIntersect_splitting(Separator *current, Separator *comparsion);
    std::vector<Separator *> splitSeparators(std::vector<Separator *> &separators, std::vector<Separator *> &finalSeparator);
    bool contain(int z, int y, int x, Separator *separator);
    bool ifIntersect_edge(CEdge *edge, Separator *separator);
    std::vector<CEdge *> splitEdges(std::vector<CEdge *> &CEdges, std::vector<Separator *> &finalSeparators);
    Block *formSingleBlock(int zStart, int yStart, int xStart, int zLim, int yLim, int xLim, char blockType,
                        std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, bool>>>> &wallMap,
                        std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, bool>>> &hmap);
    std::vector<Block *> formingBlocks(int zStart, int yStart, int xStart, int zLim, int yLim, int xLim, char blockType,
                                    std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, bool>>>> &wallMap);
    void mergeSingle(Block *cur, std::vector<Block *> &blocks);
    void mergeAdjacentBlock(std::vector<Block *> &blocks);
    void clearVariables(std::vector<CEdge *> &concaveEdges, std::vector<Separator *> walls);
public:
    /**
     * @brief Constructs a Decomp3D object.
     * 
     * @param Slices Pointer to the slices data structure.
     * @param TagTable Pointer to the tag table mapping.
     * @param Dimensions Pointer to the dimensions object.
     * Initializes the runlength_encoding with the provided parameters.
     */
    Decomp3D(std::vector<std::vector<std::vector<char>>>* Slices,
                      std::unordered_map<char, std::string>* TagTable,
                      Dimensions* Dimensions);

    /**
     * @brief Compresses an individual block within the 3D structure.
     *
     * @param x_start Starting x coordinate of the block.
     * @param y_start Starting y coordinate of the block.
     * @param z_start Starting z coordinate of the block.
     * Overrides the virtual function from the Compression class.
     */
    void CompressBlock(int x_start, int y_start, int z_start) override;
};
