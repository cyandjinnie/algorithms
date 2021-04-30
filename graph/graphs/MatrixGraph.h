//
// Created by Igor Maximov on 2019-02-04.
//

#ifndef INC_1_A_MATRIXGRAPH_H
#define INC_1_A_MATRIXGRAPH_H

#include <vector>
#include "IGraph.h"

class MatrixGraph : public IGraph {
 public:
  explicit MatrixGraph(const size_t num_vertices)
      : matrix_(num_vertices, std::vector<int>(num_vertices, 0)) {}

  explicit MatrixGraph(const IGraph* graph);

  void AddEdge(Vertex from, Vertex to) override { ++matrix_[from][to]; }

  size_t VerticesCount() const override { return matrix_.size(); }

  void GetNextVertices(Vertex vertex,
                       std::vector<Vertex>& vertices) const override;

  void GetPrevVertices(Vertex vertex,
                       std::vector<Vertex>& vertices) const override;

 private:
  std::vector<std::vector<int>> matrix_;
};

#endif  // INC_1_A_MATRIXGRAPH_H
