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
 * @brief Read the tags from the file
 * 
 * @param fin the input file stream
 * @param tags pointer to the unordered map to store the tags
 */
void read_tags(std::ifstream& fin, std::unordered_map<char, std::string>* tags) {
  std::string line;
  while (std::getline(fin, line)) {
    std::stringstream ss(line);
    std::string token;
    std::getline(ss, token, ',');
    char key = token[0];
    std::getline(ss, token, ',');
    tags->insert({key, token});
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

/**
 * @brief Print the unordered map
 * 
 * @param tags the unordered map to print
 */
void print_tags(std::unordered_map<char, std::string>& tags) {
  for (auto& tag : tags) {
    std::cout << tag.first << " " << tag.second << std::endl;
  }
}

int main(int argc, char *argv[]) {
  std::ifstream fin(argv[1]);

  std::vector<int> dimensions;
  read_dimensions(fin, &dimensions);

  print_array(dimensions);
  
  std::unordered_map<char, std::string> tags;
  read_tags(fin, &tags);
  
  print_tags(tags);

  return 0;
}
