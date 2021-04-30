//
// Created by Igor Maximov on 2019-02-04.
//

#ifndef INC_1_A_IGRAPH_H
#define INC_1_A_IGRAPH_H

#include <vector>

using std::size_t;
using Vertex = std::size_t;

struct IGraph {
 public:
  virtual ~IGraph() {}

  virtual void AddEdge(Vertex from, Vertex to) = 0;

  virtual size_t VerticesCount() const = 0;

  virtual void GetNextVertices(Vertex vertex,
                               std::vector<Vertex>& vertices) const = 0;

  virtual void GetPrevVertices(Vertex vertex,
                               std::vector<Vertex>& vertices) const = 0;
};

#endif  // INC_1_A_IGRAPH_H
