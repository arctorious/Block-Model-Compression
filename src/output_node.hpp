#pragma once

// A struct for storing sub blocks as a start coordinate and lengths of the subblock. Reused from LZW compression.
struct OutputNode
{
    int myXStart;
    int myYStart;
    int myZStart;
    int xLen;
    int yLen;
    int zLen;
    char type;
    OutputNode(): myXStart(-1), myYStart(-1), myZStart(-1), xLen(1), yLen(1), zLen(1), type(' ') {};
    OutputNode(int Xstart, int Ystart, int Zstart, int Xlen, int Ylen, int Zlen): myXStart(Xstart), myYStart(Ystart), myZStart(Zstart), xLen(Xlen), yLen(Ylen), zLen(Zlen){ type = ' ';}
    OutputNode(int Xstart, int Ystart, int Zstart, int Xlen, int Ylen, int Zlen, char Type): myXStart(Xstart), myYStart(Ystart), myZStart(Zstart), xLen(Xlen), yLen(Ylen), zLen(Zlen), type(Type){}
    void SetStart(int Xstart, int Ystart, int Zstart)
    {
        myXStart = Xstart;
        myYStart = Ystart;
        myZStart = Zstart;        
    }
    // Define equality operator for OutputNode
    bool operator==(const OutputNode& other) const 
    {
        return myXStart == other.myXStart &&
               myYStart == other.myYStart &&
               myZStart == other.myZStart &&
               type == other.type;
    }

};
