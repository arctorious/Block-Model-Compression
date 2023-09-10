#include "dp_compression.h"
#include <csignal>
#include <iostream>

template <typename T>
T min(T a, T b) {
    return std::min(a, b);
}

template <typename T, typename... Args>
T min(T a, T b, Args... args) {
    return std::min(a, min(b, args...));
}

DynamicProgrammingCompression::DynamicProgrammingCompression(std::vector<std::vector<std::vector<char>>>* Slices,
                                     std::unordered_map<char, std::string>* TagTable,
                                     Dimensions* Dimensions)
    : Compression(Slices, TagTable, Dimensions)
{}

NeighboringSameKeyStreaks DynamicProgrammingCompression::getNeighbouringSameKeyStreaks(int x, int y, int z, const std::vector<std::vector<std::vector<DPNode>>>& dp) {
    NeighboringSameKeyStreaks neighbours = {1, 1, 1}; // Initialize all values to 1 (counting the current node)
    char key = (*mySlices)[z][y][x];

    // Get the local coordinates for the dp table
    int local_z = z % myDimensions->z_parent;
    int local_y = y % myDimensions->y_parent;
    int local_x = x % myDimensions->x_parent;

    // Check for the inward neighbor
    if (local_z != 0 && (*mySlices)[z-1][y][x] == key) {
        neighbours.inward = dp[local_z-1][local_y][local_x].neighbours.inward + 1;
    }

    // Check for the up neighbor
    if (local_y != 0 && (*mySlices)[z][y-1][x] == key) {
        neighbours.up = dp[local_z][local_y-1][local_x].neighbours.up + 1;
    }

    // Check for the left neighbor
    if (local_x != 0 && (*mySlices)[z][y][x-1] == key) {
        neighbours.left = dp[local_z][local_y][local_x-1].neighbours.left + 1;
    }

    return neighbours;
}


SubBlock DynamicProgrammingCompression::findBestSubBlock(int x, int y, int z, const std::vector<std::vector<std::vector<DPNode>>>& dp) {
    
    // Get the local coordinates for the dp table
    int local_z = z % myDimensions->z_parent;
    int local_y = y % myDimensions->y_parent;
    int local_x = x % myDimensions->x_parent;

    // 1D subblocks
    SubBlock inward_1d = {1, 1, dp[local_z][local_y][local_x].neighbours.inward};
    SubBlock up_1d = {1, dp[local_z][local_y][local_x].neighbours.up, 1};
    SubBlock left_1d = {dp[local_z][local_y][local_x].neighbours.left, 1, 1};

    // 2D subblocks
    SubBlock left_up_2d = {1, 1, 1}; 
    SubBlock inward_left_2d = {1, 1, 1};
    SubBlock inward_up_2d = {1, 1, 1}; 

    // 3D subblock
    SubBlock diagonal_3d = {1, 1, 1};

    // Check for 2d left-up (ceiling) diagonal
    if (local_y != 0 && local_x != 0 && (*mySlices)[z][y-1][x-1] == (*mySlices)[z][y][x]) {
        left_up_2d.left = std::min(dp[local_z][local_y][local_x].neighbours.left, dp[local_z][local_y-1][local_x-1].sub_block.left + 1);
        left_up_2d.up = std::min(dp[local_z][local_y][local_x].neighbours.up, dp[local_z][local_y-1][local_x-1].sub_block.up + 1);
    }

    // Check for 2d inward-left diagonal
    if (local_z != 0 && local_x != 0 && (*mySlices)[z-1][y][x-1] == (*mySlices)[z][y][x]) {
        inward_left_2d.left = std::min(dp[local_z][local_y][local_x].neighbours.left, dp[local_z-1][local_y][local_x-1].sub_block.left + 1);
        inward_left_2d.inward = std::min(dp[local_z][local_y][local_x].neighbours.inward, dp[local_z-1][local_y][local_x-1].sub_block.inward + 1);
    }

    // Check for 2d inward-up diagonal
    if (local_z != 0 && local_y != 0 && (*mySlices)[z-1][y-1][x] == (*mySlices)[z][y][x]) {
        inward_up_2d.up = std::min(dp[local_z][local_y][local_x].neighbours.up, dp[local_z-1][local_y-1][local_x].sub_block.up + 1);
        inward_up_2d.inward = std::min(dp[local_z][local_y][local_x].neighbours.inward, dp[local_z-1][local_y-1][local_x].sub_block.inward + 1);
    }

    // Check for 3d diagonal
    if (local_z != 0 && local_y != 0 && local_x != 0 && (*mySlices)[z-1][y-1][x-1] == (*mySlices)[z][y][x]) {
        diagonal_3d.left = min(
            dp[local_z][local_y][local_x].neighbours.left,
            left_up_2d.left,
            inward_left_2d.left,
            dp[local_z-1][local_y-1][local_x-1].sub_block.left + 1
        );

        diagonal_3d.up = min(
            dp[local_z][local_y][local_x].neighbours.up,
            left_up_2d.up,
            inward_up_2d.up,
            dp[local_z-1][local_y-1][local_x-1].sub_block.up + 1
        );

        diagonal_3d.inward = min(
            dp[local_z][local_y][local_x].neighbours.inward,
            inward_left_2d.inward,
            inward_up_2d.inward,
            dp[local_z-1][local_y-1][local_x-1].sub_block.inward + 1
        );
    }

    // Find the best subblock
    std::array<SubBlock, 7> subBlocks = {inward_1d, up_1d, left_up_2d, left_1d, inward_left_2d, inward_up_2d, diagonal_3d};
    SubBlock bestSubBlock = subBlocks[0];
    int maxArea = bestSubBlock.left * bestSubBlock.up * bestSubBlock.inward;

    for (int l = 1; l < (signed)subBlocks.size(); l++) {
        int area = subBlocks[l].left * subBlocks[l].up * subBlocks[l].inward;
        if (area > maxArea) {
            maxArea = area;
            bestSubBlock = subBlocks[l];
        }
    }

    return bestSubBlock;
}


void DynamicProgrammingCompression::startSectioning(int x_start, int y_start, int z_start, int x_end, int y_end, int z_end, const std::vector<std::vector<std::vector<DPNode>>>& dp, std::string debug){

    // Have a base case for the recursion
    if (x_end <= x_start || y_end <= y_start || z_end <= z_start) {
        // PrintOutput(x_start+1,y_start+1,z_start+1,x_end+1,y_end+1,z_end+1,debug+" returning");
        return;
    }

    // int local_z_start = z_start % myDimensions->z_parent;
    // int local_y_start = x_start % myDimensions->x_parent;
    // int local_x_start = y_start % myDimensions->y_parent;

    int local_z_end = z_end % myDimensions->z_parent;
    int local_y_end = y_end % myDimensions->y_parent;
    int local_x_end = x_end % myDimensions->x_parent;

    // calculate bounded starting points for printing
    int printing_x_start = x_end - dp[local_z_end][local_y_end][local_x_end].sub_block.left + 1;
    int printing_y_start = y_end - dp[local_z_end][local_y_end][local_x_end].sub_block.up + 1;
    int printing_z_start = z_end - dp[local_z_end][local_y_end][local_x_end].sub_block.inward + 1;

    int true_x_length = dp[local_z_end][local_y_end][local_x_end].sub_block.left;
    int true_y_length = dp[local_z_end][local_y_end][local_x_end].sub_block.up;
    int true_z_length = dp[local_z_end][local_y_end][local_x_end].sub_block.inward;

    // PrintOutput(x_start+1,y_start+1,z_start+1,x_end+1,y_end+1,z_end+1,debug);

    if (x_start + 1 > x_end - dp[local_z_end][local_y_end][local_x_end].sub_block.left + 1){
        // std::cout<<"changed printing x start"<<std::endl;
        printing_x_start = x_start + 1;
        true_x_length = x_end - x_start;
    }
    if (y_start + 1 > y_end - dp[local_z_end][local_y_end][local_x_end].sub_block.up + 1){
        // std::cout<<"changed printing y start"<<std::endl;
        printing_y_start = y_start + 1;
        true_y_length = y_end - y_start;
    }
    if (z_start + 1 > z_end - dp[local_z_end][local_y_end][local_x_end].sub_block.inward + 1){
        // std::cout<<"changed printing z start"<<std::endl;
        printing_z_start = z_start + 1;
        true_z_length = z_end - z_start;
    }

    total_area += (true_x_length * true_y_length * true_z_length);
    
    PrintOutput(printing_x_start,
                printing_y_start,
                printing_z_start+current_level,
                true_x_length,
                true_y_length,
                true_z_length,
                getTag((*mySlices)[printing_z_start][printing_y_start][printing_x_start]));

    // PrintOutput(current_level,local_y_end,local_x_end,z_end,z_start,dp[local_z_end][local_y_end][local_x_end].sub_block.inward,"aaaaaaaaaaaaaaaaaaaa");
    // Print the (probably semi-optimal) subblock
    // PrintOutput(x_end - dp[local_z_end][local_y_end][local_x_end].sub_block.left + 1,
    //             y_end - dp[local_z_end][local_y_end][local_x_end].sub_block.up + 1,
    //             current_level + z_end - dp[local_z_end][local_y_end][local_x_end].sub_block.inward + 1,
    //             dp[local_z_end][local_y_end][local_x_end].sub_block.left,
    //             dp[local_z_end][local_y_end][local_x_end].sub_block.up,
    //             dp[local_z_end][local_y_end][local_x_end].sub_block.inward,
    //             getTag((*mySlices)[z_end][y_end][x_end]));

    // Need at least 3 recursive calls
    // Decisions, decisions, decisions...
    
    // (cube) > (plane) > (pilar)
    //
    // breaking test case
    //
    // ww
    // wo
    //
    
    // z > x > y 
    startSectioning(x_start,
                    y_start,
                    z_start,
                    x_end, 
                    y_end, 
                    z_end - true_z_length, 
                    dp,
                    debug+"↓");
    // PrintOutput(local_z_end,local_y_end,local_x_end,dp[local_z_end][local_y_end][local_x_end].sub_block.inward,
    // dp[local_z_end][local_y_end][local_x_end].sub_block.up,dp[local_z_end][local_y_end][local_x_end].sub_block.left,"aa");
    startSectioning(x_start,
                    y_start, 
                    z_end - true_z_length, 
                    x_end - true_x_length,
                    y_end, 
                    z_end, 
                    dp,
                    debug+"←");
    // PrintOutput(local_z_end,local_y_end,local_x_end,dp[local_z_end][local_y_end][local_x_end].sub_block.inward,
    // dp[local_z_end][local_y_end][local_x_end].sub_block.up,dp[local_z_end][local_y_end][local_x_end].sub_block.left,"bb");
    startSectioning(x_end - true_x_length, 
                    y_start, 
                    z_end - true_z_length,
                    x_end, 
                    y_end - true_y_length, 
                    z_end, 
                    dp,
                    debug+"↑");
    

    // z > x > y

    // y > z > x

    // y > x > z

    // x > z > y

    // x > y > z

    
}

void DynamicProgrammingCompression::CompressBlock(int x_start, int y_start, int z_start) {

    // Specifying the end indexes of this parent block
    int x_end = x_start + myDimensions->x_parent;
    int y_end = y_start + myDimensions->y_parent;
    int z_end = z_start + myDimensions->z_parent;

    // chunky boi
    std::vector<std::vector<std::vector<DPNode>>> dp(myDimensions->z_parent, std::vector<std::vector<DPNode>>(myDimensions->y_parent, std::vector<DPNode>(myDimensions->x_parent)));


    int local_x = 0;
    int local_y = 0;
    int local_z = 0;
    total_area = 0;
    for (int z = z_start; z < z_end; z++){
        for (int y = y_start; y < y_end; y++){
            for (int x = x_start; x < x_end; x++){
                dp[local_z][local_y][local_x].neighbours = getNeighbouringSameKeyStreaks(x, y, z, dp);
                dp[local_z][local_y][local_x].sub_block = findBestSubBlock(x, y, z, dp);
                local_x = (local_x + 1) % myDimensions->x_parent;
            }
            local_y = (local_y + 1) % myDimensions->y_parent;
        }
        local_z = (local_z + 1) % myDimensions->z_parent;
    }
    // PrintOutput(x_start,y_start,z_start,x_end,y_end,z_end,"--------------");
    // if (x_start == 52 && y_start == 20 && z_start == 0){
    //     std::cout<<dp[2][2][2].sub_block.left<<std::endl;
    //     std::cout<<dp[2][2][2].sub_block.up<<std::endl;
    //     std::cout<<dp[2][2][2].sub_block.inward<<std::endl;
    // }
    // else
    startSectioning(x_start-1, y_start-1, z_start-1, x_end - 1, y_end - 1, z_end - 1, dp,".");
    // if (x_start == 52 && y_start == 20 && z_start == 0){
    //     for (int z = z_start; z < z_end; z++){
    //         for (int y = y_start; y < y_end; y++){
    //             for (int x = x_start; x < x_end; x++){
    //                 std::cout << (*mySlices)[z][y][x];
    //             }
    //             std::cout<<std::endl;
    //         }
    //         std::cout<<std::endl;
    //     }
    //     raise(SIGTERM);
    // } 
}

// function that print the dp table
void DynamicProgrammingCompression::PrintDPTableSubBlocks(std::vector<std::vector<std::vector<DPNode>>>& dp){
    for (unsigned int z = 0; z < dp.size(); z++){
        for (unsigned int y = 0; y < dp[0].size(); y++){
            for (unsigned int x = 0; x < dp[0][0].size(); x++){
                std::cout<<"<"<<dp[z][y][x].sub_block.left<<" "<<dp[z][y][x].sub_block.up<<" "<<dp[z][y][x].sub_block.inward<<">"<< " ";
            }
            std::cout<<std::endl;
        }
        std::cout<<std::endl;
    }
}

// funciton to print the dp table neighbours
void DynamicProgrammingCompression::PrintDPTableNeighbours(std::vector<std::vector<std::vector<DPNode>>>& dp){
    for (unsigned int z = 0; z < dp.size(); z++){
        for (unsigned int y = 0; y < dp[0].size(); y++){
            for (unsigned int x = 0; x < dp[0][0].size(); x++){
                std::cout<<"<"<<dp[z][y][x].neighbours.left<<" "<<dp[z][y][x].neighbours.up<<" "<<dp[z][y][x].neighbours.inward<<">"<< " ";
            }
            std::cout<<std::endl;
        }
        std::cout<<std::endl;
    }
}

