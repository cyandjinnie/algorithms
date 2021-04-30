//
// Created by Igor Maximov on 2019-02-04.
//

#ifndef INC_1_A_SETGRAPH_H
#define INC_1_A_SETGRAPH_H

#include <unordered_set>
#include <vector>
#include "IGraph.h"

class SetGraph : public IGraph {
 public:
  explicit SetGraph(size_t num_vertices)
      : next_vertices_(num_vertices, std::unordered_multiset<Vertex>()),
        prev_vertices_(num_vertices, std::unordered_multiset<Vertex>()) {}

  explicit SetGraph(const IGraph* graph);

  void AddEdge(Vertex from, Vertex to) override;

  size_t VerticesCount() { return next_vertices_.size(); }

  void GetNextVertices(Vertex vertex, std::vector<Vertex>& vertices) const;

  void GetPrevVertices(Vertex vertex, std::vector<Vertex>& vertices) const;

 private:
  std::vector<std::unordered_multiset<Vertex>> next_vertices_;
  std::vector<std::unordered_multiset<Vertex>> prev_vertices_;
};

#endif  // INC_1_A_SETGRAPH_H
