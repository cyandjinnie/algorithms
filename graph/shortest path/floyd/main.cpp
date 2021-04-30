#include <iostream>
#include <set>
#include <vector>

using Vertex = std::size_t;

void FloydAlgo(std::vector<std::vector<int>>& matrix) {
  for (size_t k = 0; k < matrix.size(); ++k) {
    for (size_t i = 0; i < matrix.size(); ++i) {
      for (size_t j = 0; j < matrix.size(); ++j) {
        matrix[i][j] = std::min(matrix[i][j], matrix[i][k] + matrix[k][j]);
      }
    }
  }
}

int main() {
  size_t num_vertices = 0;
  std::cin >> num_vertices;

  std::vector<std::vector<int>> adjacency_matrix(
      num_vertices, std::vector<int>(num_vertices, 0));

  for (size_t i = 0; i < num_vertices; ++i) {
    for (size_t j = 0; j < num_vertices; ++j) {
      int weight = 0;
      std::cin >> weight;
      adjacency_matrix[i][j] = weight;
    }
  }
  FloydAlgo(adjacency_matrix);
  for (size_t i = 0; i < num_vertices; ++i) {
    for (size_t j = 0; j < num_vertices; ++j) {
      std::cout << adjacency_matrix[i][j] << ' ';
    }
    std::cout << std::endl;
  }
  return 0;
}