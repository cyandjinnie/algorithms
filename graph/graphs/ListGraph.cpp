//
// Created by Igor Maximov on 2019-02-04.
//

#include "ListGraph.h"

ListGraph::ListGraph(const IGraph* graph) {
  size_t num_vertices = graph->VerticesCount();
  next_vertices_ =
      std::vector<std::vector<Vertex>>(num_vertices, std::vector<Vertex>());
  prev_vertives_ =
      std::vector<std::vector<Vertex>>(num_vertices, std::vector<Vertex>());

  for (Vertex vertex = 0; vertex < num_vertices; ++vertex) {
    graph->GetNextVertices(vertex, next_vertices_[vertex]);
    graph->GetPrevVertices(vertex, prev_vertives_[vertex]);
  }
}

void ListGraph::AddEdge(Vertex from, Vertex to) {
  next_vertices_[from].emplace_back(to);
  prev_vertives_[to].emplace_back(from);
}

void ListGraph::GetNextVertices(Vertex vertex,
                                std::vector<Vertex>& vertices) const {
  vertices = next_vertices_[vertex];
}

void ListGraph::GetPrevVertices(Vertex vertex,
                                std::vector<Vertex>& vertices) const {
  vertices = prev_vertives_[vertex];
}
