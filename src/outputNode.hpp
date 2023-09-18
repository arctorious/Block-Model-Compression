#pragma once

struct OutputNode
{
    int xStart;
    int yStart;
    int zStart;
    int xLen;
    int yLen;
    int zLen;
    OutputNode(int Xstart, int Ystart, int Zstart, int Xlen, int Ylen, int Zlen): xStart(Xstart), yStart(Ystart), zStart(Zstart), xLen(Xlen), yLen(Ylen), zLen(Zlen){}
};