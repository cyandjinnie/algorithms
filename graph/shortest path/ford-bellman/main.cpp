#include <cassert>
#include <cmath>
#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Vertex = std::size_t;

class ListGraph {
 public:
  explicit ListGraph(const size_t num_vertices)
      : next_vertices_(num_vertices, std::vector<std::tuple<Vertex, float>>()),
        prev_vertices_(num_vertices, std::vector<std::tuple<Vertex, float>>()) {
  }

  void AddEdge(Vertex from, Vertex to, float weight);

  size_t VerticesCount() const { return next_vertices_.size(); }

  void GetNextVertices(Vertex vertex,
                       std::vector<std::tuple<Vertex, float>>& vertices) const;

  void GetPrevVertices(Vertex vertex,
                       std::vector<std::tuple<Vertex, float>>& vertices) const;

  void CondenseGraph() const;

  bool FindNegativeWeightCycle() const;

 private:
  bool FindNegativeCycleFordBellman(std::vector<Vertex>& vertices) const;

  void FindTraversalVector() const;

  void TransposedDepthFirstSearch(Vertex from) const;

  void DepthFirstSearch(Vertex from) const;

  void FindStrongConnectedComponents() const;

  mutable std::unordered_map<Vertex, Vertex> convert_to_condensed_;
  mutable std::vector<bool> visited_;
  mutable std::vector<std::vector<Vertex>> strong_connected_components_;
  mutable std::vector<Vertex> traversal_vector_;
  mutable std::vector<Vertex> current_component_;
  std::vector<std::vector<std::tuple<Vertex, float>>> next_vertices_;
  std::vector<std::vector<std::tuple<Vertex, float>>> prev_vertices_;
};

void ListGraph::AddEdge(Vertex from, Vertex to, float weight) {
  assert(0 <= from && from < VerticesCount());
  assert(0 <= to && to < VerticesCount());
  next_vertices_[from].emplace_back(to, weight);
  prev_vertices_[to].emplace_back(from, weight);
}

void ListGraph::GetNextVertices(
    Vertex vertex, std::vector<std::tuple<Vertex, float>>& vertices) const {
  vertices = next_vertices_[vertex];
}

void ListGraph::GetPrevVertices(
    Vertex vertex, std::vector<std::tuple<Vertex, float>>& vertices) const {
  vertices = prev_vertices_[vertex];
}

void ListGraph::FindTraversalVector() const {
  for (Vertex vertex = 0; vertex < VerticesCount(); ++vertex) {
    if (!visited_[vertex]) {
      TransposedDepthFirstSearch(vertex);
    }
  }
}

void ListGraph::DepthFirstSearch(Vertex from) const {
  visited_[from] = true;
  current_component_.emplace_back(from);
  for (auto [vertex, weight] : next_vertices_[from]) {
    if (!visited_[vertex]) {
      DepthFirstSearch(vertex);
    }
  }
}

void ListGraph::TransposedDepthFirstSearch(Vertex from) const {
  visited_[from] = true;
  for (auto [vertex, weight] : prev_vertices_[from]) {
    if (!visited_[vertex]) {
      TransposedDepthFirstSearch(vertex);
    }
  }
  traversal_vector_.emplace_back(from);
}

void ListGraph::CondenseGraph() const {
  visited_.assign(VerticesCount(), false);
  FindTraversalVector();
  visited_.assign(VerticesCount(), false);
  FindStrongConnectedComponents();
}

void ListGraph::FindStrongConnectedComponents() const {
  for (int64_t i = traversal_vector_.size() - 1; i >= 0; --i) {
    if (!visited_[traversal_vector_[i]]) {
      DepthFirstSearch(traversal_vector_[i]);
      strong_connected_components_.emplace_back(current_component_);
      current_component_.clear();
    }
  }
}

bool ListGraph::FindNegativeCycleFordBellman(
    std::vector<Vertex>& vertices) const {
  std::unordered_map<Vertex, bool> is_in_component;
  std::vector<std::tuple<Vertex, Vertex, float>> edges;
  std::unordered_map<Vertex, float> min_path;
  const uint64_t inf = 100000000;

  for (Vertex current = 0; current < VerticesCount(); ++current) {
    is_in_component[current] = false;
  }

  for (auto vertex : vertices) {
    is_in_component[vertex] = true;
    min_path[vertex] = inf;
  }

  for (auto current : vertices) {
    for (auto [next, weight] : next_vertices_[current]) {
      if (is_in_component[next]) {
        edges.emplace_back(next, current, weight);
      }
    }
  }

  min_path[*vertices.begin()] = 1;
  bool last_vertex_changed = false;

  for (auto vertex : vertices) {
    last_vertex_changed = false;
    for (auto [from, to, weight] : edges) {
      if (min_path[from] < inf) {
        if (min_path[to] > min_path[from] + weight) {
          min_path[to] = min_path[from] + weight;
          last_vertex_changed = true;
        }
      }
    }
  }
  return last_vertex_changed;
}

bool ListGraph::FindNegativeWeightCycle() const {
  CondenseGraph();
  for (auto component : strong_connected_components_) {
    if (FindNegativeCycleFordBellman(component)) {
      return true;
    }
  }
  return false;
}

int main() {
  size_t num_vertices = 0;
  std::cin >> num_vertices;
  auto graph = ListGraph(num_vertices);

  for (size_t i = 0; i < num_vertices; ++i) {
    for (size_t j = 0; j < num_vertices; ++j) {
      if (i != j) {
        float weight = 0;
        std::cin >> weight;
        if (weight != -1) {
          graph.AddEdge(i, j, -std::log(weight));
        }
      }
    }
  }

  std::cout << (graph.FindNegativeWeightCycle() ? "YES" : "NO") << std::endl;
  return 0;
}
