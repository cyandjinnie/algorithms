//
// Created by Igor Maximov on 2019-02-04.
//

#ifndef INC_1_A_LISTGRAPH_H
#define INC_1_A_LISTGRAPH_H

#include "IGraph.h"

class ListGraph : public IGraph {
 public:
  explicit ListGraph(const size_t num_vertices)
      : next_vertices_(num_vertices, std::vector<Vertex>()),
        prev_vertives_(num_vertices, std::vector<Vertex>()) {}

  explicit ListGraph(const IGraph* graph);

  void AddEdge(Vertex from, Vertex to) override;

  size_t VerticesCount() const override { return next_vertices_.size(); }

  void GetNextVertices(Vertex vertex,
                       std::vector<Vertex>& vertices) const override;

  void GetPrevVertices(Vertex vertex,
                       std::vector<Vertex>& vertices) const override;

 private:
  std::vector<std::vector<Vertex>> next_vertices_;
  std::vector<std::vector<Vertex>> prev_vertives_;
};

#endif  // INC_1_A_LISTGRAPH_H
