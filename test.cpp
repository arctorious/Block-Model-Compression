#include <iostream>
#include <vector>

class OutputNode {
public:
    OutputNode() : value(0) {}
    OutputNode(int value) : value(value) {}
    int getValue() const { return value; }
    void setValue(int newValue) { value = newValue; }
private:
    int value;
};

int main() {
    // Define the dimensions of the 3D vector
    const int xSize = 2;
    const int ySize = 3;
    const int zSize = 4;

    // Create an array of OutputNode objects
    OutputNode sharedNodes[xSize * ySize * zSize];

    // Initialize a 3D vector of pointers to OutputNode objects
    std::vector<std::vector<std::vector<OutputNode*>>> vector3D(
        xSize, std::vector<std::vector<OutputNode*>>(
            ySize, std::vector<OutputNode*>(
                zSize, nullptr
            )
        )
    );

    // Populate the vector with pointers to shared objects
    int index = 0;
    for (int x = 0; x < xSize; x++) {
        for (int y = 0; y < ySize; y++) {
            for (int z = 0; z < zSize; z++) {
                vector3D[x][y][z] = &sharedNodes[index++];
            }
        }
    }

    // Modify one of the shared objects
    vector3D[1][1][1] = vector3D[0][0][0];
    sharedNodes[0].setValue(42);

    // Access and print values from the 3D vector to see the changes
    std::cout << "Value at (0, 0, 0): " << vector3D[0][0][0]->getValue() << std::endl;
    std::cout << "Value at (1, 1, 1): " << vector3D[1][1][1]->getValue() << std::endl;

    return 0;
}