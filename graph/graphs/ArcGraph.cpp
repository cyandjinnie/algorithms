//
// Created by Igor Maximov on 2019-02-05.
//

#include "ArcGraph.h"

ArcGraph::ArcGraph(const IGraph* graph) {
  num_vertices_ = graph->VerticesCount();
  for (size_t from = 0; from < num_vertices_; ++from) {
    auto temp = std::vector<Vertex>();
    graph->GetNextVertices(from, temp);
    for (auto to : temp) {
      edges_.emplace_back(std::make_pair(from, to));
    }
  }
}

void ArcGraph::AddEdge(Vertex from, Vertex to) {
  edges_.emplace_back(std::make_pair(from, to));
}

void ArcGraph::GetNextVertices(Vertex vertex,
                               std::vector<Vertex>& vertices) const {
  vertices.clear();
  for (auto [from, to] : edges_) {
    if (from == vertex) {
      vertices.emplace_back(to);
    }
  }
}

void ArcGraph::GetPrevVertices(Vertex vertex,
                               std::vector<Vertex>& vertices) const {
  vertices.clear();
  for (auto [from, to] : edges_) {
    if (from == vertex) {
      vertices.emplace_back(to);
    }
  }
}