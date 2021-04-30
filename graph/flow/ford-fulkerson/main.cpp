#include <cassert>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

using Vertex = int;
using flow_type = int;

class ListGraph {
 public:
  explicit ListGraph(const size_t num_vertices)
      : next_vertices_(num_vertices, std::vector<Vertex>()),
        prev_vertives_(num_vertices, std::vector<Vertex>()),
        capacity(num_vertices, std::vector<flow_type>(num_vertices, 0)) {}

  void AddEdge(Vertex from, Vertex to, flow_type maxflow);

  size_t VerticesCount() const { return next_vertices_.size(); }

  void GetNextVertices(
      Vertex vertex, std::vector<std::pair<Vertex, flow_type>>& vertices) const;

  bool ShortestPathInResidualNetworkExists(
      Vertex from, Vertex to,
      std::unordered_map<Vertex, Vertex>& parents) const;

  bool FindAugmentingPath(Vertex from, Vertex to,
                          flow_type& path_maxflow) const;

  size_t FindMaxFlow(Vertex from, Vertex to) const;

 private:
  std::vector<std::vector<Vertex>> next_vertices_;
  std::vector<std::vector<Vertex>> prev_vertives_;
  mutable std::vector<std::vector<flow_type>> capacity;
  mutable std::vector<std::vector<flow_type>> residue_network;
};

void ListGraph::AddEdge(Vertex from, Vertex to, flow_type maxflow) {
  assert(0 <= from && from < VerticesCount());
  assert(0 <= to && to < VerticesCount());
  capacity[from][to] = maxflow;
}

void ListGraph::GetNextVertices(
    Vertex vertex, std::vector<std::pair<Vertex, flow_type>>& vertices) const {
  vertices.clear();
  for (Vertex to = 0; to < VerticesCount(); ++to) {
    if (residue_network[vertex][to] != 0) {
      vertices.emplace_back(to, residue_network[vertex][to]);
    }
  }
}

bool ListGraph::FindAugmentingPath(Vertex from, Vertex to,
                                   flow_type& path_maxflow) const {
  std::unordered_map<Vertex, Vertex> parents;
  bool path_exists = ShortestPathInResidualNetworkExists(from, to, parents);
  if (path_exists) {
    Vertex current = to;
    flow_type min_capacity = 0;
    bool found = false;

    while (current != from) {
      Vertex previous = parents[current];
      if (!found) {
        min_capacity = residue_network[previous][current];
        found = true;
      } else {
        min_capacity =
            std::min(min_capacity, residue_network[previous][current]);
      }
      current = previous;
    }

    current = to;
    while (current != from) {
      Vertex previous = parents[current];
      residue_network[previous][current] -= min_capacity;
      residue_network[current][previous] += min_capacity;
      current = previous;
    }
    path_maxflow = min_capacity;
  }
  return path_exists;
}

size_t ListGraph::FindMaxFlow(Vertex from, Vertex to) const {
  residue_network = capacity;
  flow_type path_maxflow = 0;
  size_t max_flow = 0;
  while (FindAugmentingPath(from, to, path_maxflow)) {
    max_flow += path_maxflow;
  }
  return max_flow;
}

bool ListGraph::ShortestPathInResidualNetworkExists(
    Vertex from, Vertex to, std::unordered_map<Vertex, Vertex>& parents) const {
  std::queue<Vertex> vertices_queue;
  std::vector<bool> visited(VerticesCount(), false);
  std::vector<std::pair<Vertex, flow_type>> next_vertices;
  vertices_queue.push(from);
  parents[from] = from;
  while (!vertices_queue.empty()) {
    Vertex current = vertices_queue.front();
    vertices_queue.pop();
    if (current == to) {
      return true;
    }
    GetNextVertices(current, next_vertices);
    for (auto [next, edge_capacity] : next_vertices) {
      if (!visited[next] && (edge_capacity != 0)) {
        vertices_queue.push(next);
        parents[next] = current;
        visited[next] = true;
      }
    }
  }
  return false;
}

int main() {
  size_t num_vertices = 0;
  size_t num_edges = 0;
  std::cin >> num_vertices;
  std::cin >> num_edges;
  auto graph = ListGraph(num_vertices);
  for (size_t i = 0; i < num_edges; ++i) {
    Vertex from = 0;
    Vertex to = 0;
    flow_type maxflow = 0;
    std::cin >> from >> to >> maxflow;
    graph.AddEdge(from - 1, to - 1, maxflow);
  }
  std::cout << graph.FindMaxFlow(0, num_vertices - 1);
}