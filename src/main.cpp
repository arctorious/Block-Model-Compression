#include <bits/stdc++.h>

/**
 * @brief Read the x_count, y_count, z_count, parent_x, parent_y, parent_z from
 * the file
 *
 * @param fin the input file stream
 * @param dimensions pointer to the array to store the dimensions
 */
void read_dimensions(std::ifstream& fin, std::vector<int>* dimensions) {
  std::string line;
  std::getline(fin, line);
  std::stringstream ss(line);
  std::string token;
  while (std::getline(ss, token, ',')) {
    dimensions->push_back(std::stoi(token));
  }
}

/**
 * @brief Print the array
 *
 * @param dimensions the array to print
 */
void print_array(std::vector<int>& dimensions) {
  for (int i = 0; i < dimensions.size(); i++) {
    std::cout << dimensions[i] << " ";
  }
  std::cout << std::endl;
}

int main(int argc, char *argv[]) {
  std::ifstream fin(argv[1]);

  std::vector<int> dimensions;
  read_dimensions(fin, &dimensions);

  print_array(dimensions);

  return 0;
}
