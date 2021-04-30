#include <cassert>
#include <iostream>
#include <queue>
#include <set>
#include <tuple>
#include <unordered_map>
#include <vector>

using Vertex = size_t;

class ListGraph {
 public:
  explicit ListGraph(const int num_vertices)
      : next_vertices_(num_vertices, std::vector<std::pair<Vertex, size_t>>()) {
  }

  void AddEdge(Vertex from, Vertex to, size_t weight);

  size_t VerticesCount() const { return next_vertices_.size(); }

  void GetNextVertices(Vertex vertex,
                       std::vector<std::pair<Vertex, size_t>>& vertices) const;

  int MinSpanningTreeWeight() const;

 private:
  std::vector<std::vector<std::pair<Vertex, size_t>>> next_vertices_;
};

void ListGraph::AddEdge(Vertex from, Vertex to, size_t weight) {
  assert(0 <= from && from < VerticesCount());
  assert(0 <= to && to < VerticesCount());
  next_vertices_[from].emplace_back(to, weight);
}

void ListGraph::GetNextVertices(
    Vertex vertex, std::vector<std::pair<Vertex, size_t>>& vertices) const {
  vertices = next_vertices_[vertex];
}

int ListGraph::MinSpanningTreeWeight() const {
  const size_t inf = std::numeric_limits<size_t>::max();
  int weight_sum = 0;
  // (weight, from, to)
  std::set<std::pair<size_t, Vertex>> edges;
  std::vector<size_t> min_edge(VerticesCount(), inf);
  std::vector<bool> is_added(VerticesCount(), false);
  min_edge[0] = 0;
  is_added[0] = true;
  edges.emplace(0, 0);
  for (int edges_added = 0; edges_added < VerticesCount(); ++edges_added) {
    auto [key, current] = *edges.begin();
    edges.erase(edges.begin());
    weight_sum += key;
    is_added[current] = true;
    for (auto [next, weight] : next_vertices_[current]) {
      if (weight < min_edge[next] && !is_added[next]) {
        edges.erase(std::make_pair(min_edge[next], next));
        min_edge[next] = weight;
        edges.emplace(min_edge[next], next);
      }
    }
  }
  return weight_sum;
}

int main() {
  size_t num_vertices = 0;
  size_t num_edges = 0;
  std::cin >> num_vertices >> num_edges;
  ListGraph graph(num_vertices);
  for (int i = 0; i < num_edges; ++i) {
    Vertex from = 0;
    Vertex to = 0;
    size_t weight = 0;
    std::cin >> from >> to >> weight;
    graph.AddEdge(from - 1, to - 1, weight);
    graph.AddEdge(to - 1, from - 1, weight);
  }
  std::cout << graph.MinSpanningTreeWeight() << std::endl;
}