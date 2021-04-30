//
// Created by Igor Maximov on 2019-02-05.
//

#ifndef INC_1_A_ARCGRAPH_H
#define INC_1_A_ARCGRAPH_H

#include <set>
#include <vector>
#include "IGraph.h"

class ArcGraph : public IGraph {
 public:
  explicit ArcGraph(size_t num_vertices) : num_vertices_(num_vertices) {}

  explicit ArcGraph(const IGraph* graph);

  void AddEdge(Vertex from, Vertex to) override;

  size_t VerticesCount() const override { return num_vertices_; }

  void GetNextVertices(Vertex vertex,
                       std::vector<Vertex>& vertices) const override;

  void GetPrevVertices(Vertex vertex,
                       std::vector<Vertex>& vertices) const override;

 private:
  size_t num_vertices_;
  std::vector<std::pair<Vertex, Vertex>> edges_;
};

#endif  // INC_1_A_ARCGRAPH_H
