#include <cassert>
#include <iostream>
#include <optional>
#include <queue>
#include <set>
#include <tuple>
#include <vector>

using Vertex = std::size_t;

struct DijkstraVertexInfo {
  DijkstraVertexInfo()
      : visited(false), is_infinity(true), is_in_queue(false) {}
  bool visited;
  bool is_infinity;
  bool is_in_queue;
  size_t min_way;
};

void Relax(std::vector<DijkstraVertexInfo>& info, Vertex next, Vertex current,
           size_t weight) {
  if (info[next].is_infinity) {
    info[next].min_way = info[current].min_way + weight;
    info[next].is_infinity = false;
  } else {
    info[next].min_way =
        std::min(info[current].min_way + weight, info[next].min_way);
  }
}

size_t FindMinPath(std::vector<std::vector<std::tuple<Vertex, size_t>>>& list,
                   Vertex from, Vertex to) {
  std::vector<DijkstraVertexInfo> info(list.size());
  size_t visited_cnt = 0;

  // Setup comparator for set with min_ways closure
  auto comparator = [&](const Vertex& lhs, const Vertex& rhs) {
    return info[lhs].min_way == info[rhs].min_way
               ? lhs < rhs
               : info[lhs].min_way < info[rhs].min_way;
  };
  auto weights = std::set<Vertex, decltype(comparator)>(comparator);

  // Initialize
  info[from].min_way = 0;
  weights.insert(from);
  info[from].is_infinity = false;
  info[from].is_in_queue = true;

  while (visited_cnt < list.size() && !weights.empty()) {
    // Find vertex in vector w/ least weight
    Vertex current = *weights.begin();
    weights.erase(current);

    // Relax adjacent vertices
    for (auto [next, weight] : list[current]) {
      if (!info[next].visited) {
        if (info[next].is_in_queue) {
          weights.erase(next);
        }
        Relax(info, next, current, weight);
        if (!info[next].is_in_queue) {
          info[next].is_in_queue = true;
        }
        weights.insert(next);
      }
    }
    ++visited_cnt;
    info[current].visited = true;
  }
  return info[to].min_way;
}

int main() {
  size_t num_vertices = 0;
  size_t num_edges = 0;
  std::cin >> num_vertices;
  std::cin >> num_edges;

  std::vector<std::vector<std::tuple<Vertex, size_t>>> adjacency_list(
      num_vertices, std::vector<std::tuple<Vertex, size_t>>());

  for (size_t i = 0; i < num_edges; ++i) {
    Vertex from = 0;
    Vertex to = 0;
    int weight = 0;
    std::cin >> from >> to >> weight;
    adjacency_list[from].emplace_back(to, weight);
    adjacency_list[to].emplace_back(from, weight);
  }

  Vertex from = 0;
  Vertex to = 0;
  std::cin >> from >> to;

  std::cout << FindMinPath(adjacency_list, from, to) << std::endl;
  return 0;
}
