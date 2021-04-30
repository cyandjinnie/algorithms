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

  std::pair<size_t, bool> CycleSearch(Vertex vertex) const;

  int64_t MinCycle() const;

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

std::pair<size_t, bool> ListGraph::CycleSearch(Vertex start_vertex) const {
  auto visited = std::vector<bool>(VerticesCount(), false);
  auto previous_length = std::vector<size_t>(VerticesCount(), 0);
  auto queue = std::queue<std::pair<Vertex, size_t>>();
  queue.push(std::make_pair(start_vertex, 0));
  while (!queue.empty()) {
    auto [vertex, level] = queue.front();
    // std::cout << current << " ";
    if (visited[vertex]) {
      return std::make_pair(level + previous_length[vertex], true);
    }
    previous_length[vertex] = level;
    visited[vertex] = true;
    queue.pop();
    for (auto i : next_vertices_[vertex]) {
      if (!visited[i]) {
        queue.push(std::make_pair(i, level + 1));
      }
    }
  }
  return std::make_pair(-1, false);
}

int64_t ListGraph::MinCycle() const {
  size_t min_cycle = 0;
  bool found_cycle = false;
  for (size_t i = 1; i < VerticesCount(); ++i) {
    auto [temp_length, temp_flag] = CycleSearch(i);
    if (!found_cycle) {
      if (temp_flag) {
        min_cycle = temp_length;
        found_cycle = true;
      }
    } else {
      if (temp_length < min_cycle) {
        min_cycle = temp_length;
      }
    }
  }
  return found_cycle ? min_cycle : -1;
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
    std::cin >> from >> to;
    graph.AddEdge(from, to);
    graph.AddEdge(to, from);
  }
  std::cout << graph.MinCycle() << std::endl;
}
