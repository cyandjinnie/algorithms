//
// Created by Igor Maximov on 2019-02-04.
//

#include "MatrixGraph.h"

MatrixGraph::MatrixGraph(const IGraph* graph) {
  size_t num_vertices = graph->VerticesCount();
  matrix_ = std::vector<std::vector<int>>(num_vertices,
                                          std::vector<int>(num_vertices, 0));
  auto temp = std::vector<Vertex>();
  for (int i = 0; i < num_vertices; ++i) {
    graph->GetNextVertices(i, temp);
    for (int j = 0; j < temp.size(); ++i) {
      ++matrix_[i][j];
    }
  }
}

void MatrixGraph::GetPrevVertices(Vertex vertex,
                                  std::vector<Vertex>& vertices) const {
  vertices = std::vector<Vertex>();
  for (int i = 0; i < MatrixGraph::VerticesCount(); ++i) {
    if (matrix_[i][vertex] != 0) {
      vertices.push_back(i);
    }
  }
}

void MatrixGraph::GetNextVertices(Vertex vertex,
                                  std::vector<Vertex>& vertices) const {
  vertices = std::vector<Vertex>();
  for (int i = 0; i < MatrixGraph::VerticesCount(); ++i) {
    if (matrix_[vertex][i] != 0) {
      vertices.push_back(i);
    }
  }
}