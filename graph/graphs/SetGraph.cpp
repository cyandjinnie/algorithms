//
// Created by Igor Maximov on 2019-02-04.
//

#include "SetGraph.h"

SetGraph::SetGraph(const IGraph *graph) {
  size_t num_vertices = graph->VerticesCount();
  for (int i = 0; i < num_vertices; ++i) {
    auto temp = std::vector<Vertex>();
    graph->GetNextVertices(i, temp);
    next_vertices_[i] =
        std::unordered_multiset<Vertex>(temp.begin(), temp.end());
    graph->GetPrevVertices(i, temp);
    prev_vertices_[i] =
        std::unordered_multiset<Vertex>(temp.begin(), temp.end());
  }
}

void SetGraph::AddEdge(Vertex from, Vertex to) {
  next_vertices_[from].insert(to);
  prev_vertices_[to].insert(from);
}

void SetGraph::GetNextVertices(Vertex vertex,
                               std::vector<Vertex> &vertices) const {
  vertices = std::vector<Vertex>(next_vertices_[vertex].begin(),
                                 next_vertices_[vertex].end());
}

void SetGraph::GetPrevVertices(Vertex vertex,
                               std::vector<Vertex> &vertices) const {
  vertices = std::vector<Vertex>(prev_vertices_[vertex].begin(),
                                 prev_vertices_[vertex].end());
}
