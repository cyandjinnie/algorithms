#include <cassert>
#include <iostream>
#include <queue>
#include <vector>

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

class ListGraph : public IGraph {
 public:
  explicit ListGraph(const size_t num_vertices)
      : next_vertices_(num_vertices, std::vector<Vertex>()),
        prev_vertives_(num_vertices, std::vector<Vertex>()) {}

  explicit ListGraph(const IGraph* graph);

  void AddEdge(Vertex from, Vertex to) override;

  size_t VerticesCount() const override { return next_vertices_.size(); }

  void GetNextVertices(Vertex vertex,
                       std::vector<Vertex>& vertices) const override;

  void GetPrevVertices(Vertex vertex,
                       std::vector<Vertex>& vertices) const override;

  size_t CountMinWays(Vertex initial, Vertex target) const;

 private:
  std::vector<std::vector<Vertex>> next_vertices_;
  std::vector<std::vector<Vertex>> prev_vertives_;
};

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
  assert(0 <= from && from < VerticesCount());
  assert(0 <= to && to < VerticesCount());
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

size_t ListGraph::CountMinWays(Vertex initial, Vertex target) const {
  auto visited = std::vector<bool>(VerticesCount());
  auto ways = std::vector<size_t>(VerticesCount(), 0);
  auto min_time = std::vector<size_t>(VerticesCount(), 0);
  auto queue = std::queue<std::pair<Vertex, size_t>>();
  ways[initial] = 1;
  visited[initial] = true;
  for (auto next : next_vertices_[initial]) {
    if (!visited[next]) {
      queue.push(std::make_pair(next, initial));
    }
  }
  while (!queue.empty()) {
    auto [current, previous] = queue.front();
    queue.pop();
    if (!visited[current]) {
      min_time[current] = min_time[previous] + 1;
      visited[current] = true;
    } else {
      if (min_time[current] == min_time[previous] + 1) {
        ways[current] += ways[previous];
      }
      continue;
    }
    ways[current] += ways[previous];
    for (auto next : next_vertices_[current]) {
      if (!visited[next]) {
        queue.push(std::make_pair(next, current));
      }
    }
  }
  return ways[target];
}

int main() {
  size_t num_vertices = 0;
  size_t num_edges = 0;
  std::cin >> num_vertices >> num_edges;
  auto graph = ListGraph(num_vertices);
  for (size_t i = 0; i < num_edges; ++i) {
    Vertex from = 0;
    Vertex to = 0;
    std::cin >> from >> to;
    graph.AddEdge(from, to);
    graph.AddEdge(to, from);
  }
  Vertex from = 0;
  Vertex to = 0;
  std::cin >> from;
  std::cin >> to;
  std::cout << std::endl;
  std::cout << graph.CountMinWays(from, to);
  return 0;
}
