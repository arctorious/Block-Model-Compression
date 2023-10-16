#include "dp_compression.h"

template <typename T>
T min(T a, T b)
{
    return std::min(a, b);
}

template <typename T, typename... Args>
T min(T a, T b, Args... args)
{
    return std::min(a, min(b, args...));
}

// Overload the += operator for std::vector<PrintNode>
std::vector<PrintNode> &operator+=(std::vector<PrintNode> &a, const std::vector<PrintNode> &b)
{
    a.insert(a.end(), b.begin(), b.end());
    return a;
}

DynamicProgrammingCompression::DynamicProgrammingCompression(std::vector<std::vector<std::vector<char>>> *Slices,
                                                             std::unordered_map<char, std::string> *TagTable,
                                                             Dimensions *Dimensions)
    : Compression(Slices, TagTable, Dimensions)
{
}

NeighboringSameKeyStreaks DynamicProgrammingCompression::getNeighbouringSameKeyStreaks(int x, int y, int z, const std::vector<std::vector<std::vector<DPNode>>> &dp)
{
    NeighboringSameKeyStreaks neighbours = {1, 1, 1}; // Initialize all values to 1 (counting the current node)
    char key = (*mySlices)[z][y][x];

    // Get the local coordinates for the dp table
    int local_z = z % myDimensions->z_parent;
    int local_y = y % myDimensions->y_parent;
    int local_x = x % myDimensions->x_parent;

    // Check for the inward neighbor
    if (local_z != 0 && (*mySlices)[z - 1][y][x] == key)
    {
        neighbours.inward = dp[local_z - 1][local_y][local_x].neighbours.inward + 1;
    }

    // Check for the up neighbor
    if (local_y != 0 && (*mySlices)[z][y - 1][x] == key)
    {
        neighbours.up = dp[local_z][local_y - 1][local_x].neighbours.up + 1;
    }

    // Check for the left neighbor
    if (local_x != 0 && (*mySlices)[z][y][x - 1] == key)
    {
        neighbours.left = dp[local_z][local_y][local_x - 1].neighbours.left + 1;
    }

    return neighbours;
}

SubBlock DynamicProgrammingCompression::findBestSubBlock(int x, int y, int z, const std::vector<std::vector<std::vector<DPNode>>> &dp)
{

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
    if (local_y != 0 && local_x != 0 && (*mySlices)[z][y - 1][x - 1] == (*mySlices)[z][y][x])
    {
        left_up_2d.left = std::min(dp[local_z][local_y][local_x].neighbours.left, dp[local_z][local_y - 1][local_x - 1].sub_block.left + 1);
        left_up_2d.up = std::min(dp[local_z][local_y][local_x].neighbours.up, dp[local_z][local_y - 1][local_x - 1].sub_block.up + 1);
    }

    // Check for 2d inward-left diagonal
    if (local_z != 0 && local_x != 0 && (*mySlices)[z - 1][y][x - 1] == (*mySlices)[z][y][x])
    {
        inward_left_2d.left = std::min(dp[local_z][local_y][local_x].neighbours.left, dp[local_z - 1][local_y][local_x - 1].sub_block.left + 1);
        inward_left_2d.inward = std::min(dp[local_z][local_y][local_x].neighbours.inward, dp[local_z - 1][local_y][local_x - 1].sub_block.inward + 1);
    }

    // Check for 2d inward-up diagonal
    if (local_z != 0 && local_y != 0 && (*mySlices)[z - 1][y - 1][x] == (*mySlices)[z][y][x])
    {
        inward_up_2d.up = std::min(dp[local_z][local_y][local_x].neighbours.up, dp[local_z - 1][local_y - 1][local_x].sub_block.up + 1);
        inward_up_2d.inward = std::min(dp[local_z][local_y][local_x].neighbours.inward, dp[local_z - 1][local_y - 1][local_x].sub_block.inward + 1);
    }

    // Check for 3d diagonal
    if (local_z != 0 && local_y != 0 && local_x != 0 && (*mySlices)[z - 1][y - 1][x - 1] == (*mySlices)[z][y][x])
    {
        diagonal_3d.left = min(
            dp[local_z][local_y][local_x].neighbours.left,
            left_up_2d.left,
            inward_left_2d.left,
            dp[local_z - 1][local_y - 1][local_x - 1].sub_block.left + 1);

        diagonal_3d.up = min(
            dp[local_z][local_y][local_x].neighbours.up,
            left_up_2d.up,
            inward_up_2d.up,
            dp[local_z - 1][local_y - 1][local_x - 1].sub_block.up + 1);

        diagonal_3d.inward = min(
            dp[local_z][local_y][local_x].neighbours.inward,
            inward_left_2d.inward,
            inward_up_2d.inward,
            dp[local_z - 1][local_y - 1][local_x - 1].sub_block.inward + 1);
    }

    // Find the best subblock
    std::array<SubBlock, 7> subBlocks = {inward_1d, up_1d, left_up_2d, left_1d, inward_left_2d, inward_up_2d, diagonal_3d};
    SubBlock bestSubBlock = subBlocks[0];
    int maxArea = bestSubBlock.left * bestSubBlock.up * bestSubBlock.inward;

    for (int l = 1; l < (signed)subBlocks.size(); l++)
    {
        int area = subBlocks[l].left * subBlocks[l].up * subBlocks[l].inward;
        if (area > maxArea)
        {
            maxArea = area;
            bestSubBlock = subBlocks[l];
        }
    }

    return bestSubBlock;
}

// Helper function to calculate the printing start and true length for each axis
void calculateBoundedSubBlock(int &printing_start, int &true_length, int start, int end, int dp_length)
{
    printing_start = end - dp_length + 1;
    true_length = dp_length;

    if (start + 1 > printing_start)
    {
        printing_start = start + 1;
        true_length = end - start;
    }
}

std::vector<PrintNode> DynamicProgrammingCompression::startSectioning(int x_start, int y_start, int z_start, int x_end, int y_end, int z_end, const std::vector<std::vector<std::vector<DPNode>>> &dp)
{

    // Have a base case for the recursion
    if (x_end <= x_start || y_end <= y_start || z_end <= z_start)
    {
        return {};
    }

    int local_z_end = z_end % myDimensions->z_parent;
    int local_y_end = y_end % myDimensions->y_parent;
    int local_x_end = x_end % myDimensions->x_parent;

    // Calculate bounded starting points and true lengths for printing
    int bounded_x_start, bounded_y_start, bounded_z_start;
    int bounded_x_length, bounded_y_length, bounded_z_length;

    SubBlock sub = dp[local_z_end][local_y_end][local_x_end].sub_block;

    calculateBoundedSubBlock(bounded_x_start, bounded_x_length, x_start, x_end, sub.left);
    calculateBoundedSubBlock(bounded_y_start, bounded_y_length, y_start, y_end, sub.up);
    calculateBoundedSubBlock(bounded_z_start, bounded_z_length, z_start, z_end, sub.inward);

    int local_bounded_x_start = bounded_x_start % myDimensions->x_parent;
    int local_bounded_y_start = bounded_y_start % myDimensions->y_parent;
    int local_bounded_z_start = bounded_z_start % myDimensions->z_parent;

    // prtin bounded_x_start, bounded_y_start, bounded_z_start
    // std::cout<<bounded_x_start<<" "<<bounded_y_start<<" "<<bounded_z_start<<std::endl;


    std::vector<PrintNode> currSubBlock;
    currSubBlock.push_back({local_bounded_x_start,
                         local_bounded_y_start,
                         local_bounded_z_start,
                         bounded_x_length,
                         bounded_y_length,
                         bounded_z_length});

    // all the possible ways to section the block
    int sectioningConfig[6][3][6] = {
        {{x_start, y_start, z_start, x_end, y_end, z_end - bounded_z_length},
         {x_start, y_start, z_end - bounded_z_length, x_end - bounded_x_length, y_end, z_end},
         {x_end - bounded_x_length, y_start, z_end - bounded_z_length, x_end, y_end - bounded_y_length, z_end}},
        {{x_start, y_start, z_start, x_end, y_end, z_end - bounded_z_length},
         {x_start, y_start, z_end - bounded_z_length, x_end, y_end - bounded_y_length, z_end},
         {x_start, y_end - bounded_y_length, z_end - bounded_z_length, x_end - bounded_x_length, y_end, z_end}},
        {{x_start, y_start, z_start, x_end - bounded_x_length, y_end, z_end},
         {x_end - bounded_x_length, y_start, z_start, x_end, y_end, z_end - bounded_z_length},
         {x_end - bounded_x_length, y_start, z_end - bounded_z_length, x_end, y_end - bounded_y_length, z_end}},
        {{x_start, y_start, z_start, x_end - bounded_x_length, y_end, z_end},
         {x_end - bounded_x_length, y_start, z_start, x_end, y_end - bounded_y_length, z_end},
         {x_end - bounded_x_length, y_end - bounded_y_length, z_start, x_end, y_end, z_end - bounded_z_length}},
        {{x_start, y_start, z_start, x_end, y_end - bounded_y_length, z_end},
         {x_start, y_end - bounded_y_length, z_start, x_end - bounded_x_length, y_end, z_end},
         {x_end - bounded_x_length, y_end - bounded_y_length, z_start, x_end, y_end, z_end - bounded_z_length}},
        {{x_start, y_start, z_start, x_end, y_end - bounded_y_length, z_end},
         {x_start, y_end - bounded_y_length, z_start, x_end, y_end, z_end - bounded_z_length},
         {x_start, y_end - bounded_y_length, z_end - bounded_z_length, x_end - bounded_x_length, y_end, z_end}}};

    size_t minSize = SIZE_MAX;
    std::vector<PrintNode> leastSubBlockPath;

              // change ↓ to 6 to test all the sectioningConfig
    for (int i = 0; i < 1; i++)
    {
        std::vector<PrintNode> temp;
        for (int j = 0; j < 3; j++)
        {
            temp += startSectioning(sectioningConfig[i][j][0],
                                    sectioningConfig[i][j][1],
                                    sectioningConfig[i][j][2],
                                    sectioningConfig[i][j][3],
                                    sectioningConfig[i][j][4],
                                    sectioningConfig[i][j][5],
                                    dp);
        }
        if (temp.size() < minSize)
        {
            minSize = temp.size();
            leastSubBlockPath = temp;
        }
    }
    return currSubBlock += leastSubBlockPath;
}

// Function to hash a DPNode
std::size_t hashDPNode(const DPNode& node) {
    std::size_t seed = 17;  // A prime number
    seed = seed * 31 + std::hash<int>()(node.neighbours.left);  // 31 is also a prime number
    seed = seed * 31 + std::hash<int>()(node.neighbours.up);
    seed = seed * 31 + std::hash<int>()(node.neighbours.inward);
    seed = seed * 31 + std::hash<int>()(node.sub_block.left);
    seed = seed * 31 + std::hash<int>()(node.sub_block.up);
    seed = seed * 31 + std::hash<int>()(node.sub_block.inward);
    return seed;
}

// Function to hash a DP table
std::size_t hashDPTable(const std::vector<std::vector<std::vector<DPNode>>>& dp) {
    std::size_t seed = 17;
        for (const auto& plane : dp) {
            for (const auto& row : plane) {
                for (const auto& cell : row) {
                    std::size_t node_hash = hashDPNode(cell);
                    seed = seed * 31 + node_hash;
                }
            }
        }
    return seed;
}

// Function to insert DP table hash into the map
void DynamicProgrammingCompression::insertDPTable(std::size_t hash_val, std::vector<PrintNode> prints) {
    std::lock_guard<std::mutex> lock(map_mutex); // Lock the mutex
    map[hash_val] = prints;
}

void DynamicProgrammingCompression::printPrintNodes(int x_start, int y_start, int z_start, std::vector<PrintNode> prints) {
    for (unsigned int i = 0; i < prints.size(); i++)
    {
        int x_print = x_start + prints[i].x_position;
        int y_print = y_start + prints[i].y_position;
        int z_print = z_start + prints[i].z_position;
        std::string label = getTag((*mySlices)[z_print][y_print][x_print]);
        PrintOutput(x_print,
                    y_print,
                    z_print + current_level,
                    prints[i].x_size,
                    prints[i].y_size,
                    prints[i].z_size,
                    label);
    }
}

void DynamicProgrammingCompression::CompressBlock(int x_start, int y_start, int z_start)
{

    // Specifying the end indexes of this parent block
    int x_end = x_start + myDimensions->x_parent;
    int y_end = y_start + myDimensions->y_parent;
    int z_end = z_start + myDimensions->z_parent;

    // Initialize the dp table
    std::vector<std::vector<std::vector<DPNode>>> dp(myDimensions->z_parent, 
                    std::vector<std::vector<DPNode>>(myDimensions->y_parent, 
                                 std::vector<DPNode>(myDimensions->x_parent)));

    int local_x = 0;
    int local_y = 0;
    int local_z = 0;
    
    for (int z = z_start; z < z_end; z++)
    {
        for (int y = y_start; y < y_end; y++)
        {
            for (int x = x_start; x < x_end; x++)
            {
                dp[local_z][local_y][local_x].neighbours = getNeighbouringSameKeyStreaks(x, y, z, dp);
                dp[local_z][local_y][local_x].sub_block = findBestSubBlock(x, y, z, dp);
                local_x = (local_x + 1) % myDimensions->x_parent;
            }
            local_y = (local_y + 1) % myDimensions->y_parent;
        }
        local_z = (local_z + 1) % myDimensions->z_parent;
    }

    // Hash the dp table
    std::size_t hash_val = hashDPTable(dp);
    auto it = map.find(hash_val);

    if (it != map.end()) {
        std::vector<PrintNode> prints = it->second;
        printPrintNodes(x_start, y_start, z_start, prints);
    }
    else{
        std::vector<PrintNode> prints = startSectioning(x_start - 1, y_start - 1, z_start - 1, x_end - 1, y_end - 1, z_end - 1, dp);
        insertDPTable(hash_val, prints);
        printPrintNodes(x_start, y_start, z_start, prints);
    }

    // if (total_area != myDimensions->z_parent * myDimensions->y_parent * myDimensions->x_parent){
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

void DynamicProgrammingCompression::PrintDPTableSubBlocks(std::vector<std::vector<std::vector<DPNode>>> &dp)
{
    for (unsigned int z = 0; z < dp.size(); z++)
    {
        for (unsigned int y = 0; y < dp[0].size(); y++)
        {
            for (unsigned int x = 0; x < dp[0][0].size(); x++)
            {
                std::cout << "<" << dp[z][y][x].sub_block.left << " " << dp[z][y][x].sub_block.up << " " << dp[z][y][x].sub_block.inward << ">"
                          << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

void DynamicProgrammingCompression::PrintDPTableNeighbours(std::vector<std::vector<std::vector<DPNode>>> &dp)
{
    for (unsigned int z = 0; z < dp.size(); z++)
    {
        for (unsigned int y = 0; y < dp[0].size(); y++)
        {
            for (unsigned int x = 0; x < dp[0][0].size(); x++)
            {
                std::cout << "<" << dp[z][y][x].neighbours.left << " " << dp[z][y][x].neighbours.up << " " << dp[z][y][x].neighbours.inward << ">"
                          << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}
