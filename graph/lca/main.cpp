#include <cmath>
#include <iostream>
#include <vector>

using Vertex = int;

//#define DEBUG

class LeastCommonAncestorSolver {
 public:
  explicit LeastCommonAncestorSolver(
      const std::vector<std::vector<Vertex>>& tree)
      : current_time_(0),
        tree_(tree),
        num_vertices_(tree.size()),
        ancestors_table_depth_(static_cast<size_t>(log2(num_vertices_))),
        time_in_(num_vertices_),
        time_out_(num_vertices_),
        ancestors_sparse_table_(
            num_vertices_, std::vector<Vertex>(ancestors_table_depth_ + 1)) {
    BuildTableDFS(0, 0);

#ifdef DEBUG
    for (auto&& row : ancestors_sparse_table_) {
      for (auto&& col : row) {
        std::cout << col << " ";
      }
      std::cout << std::endl;
    }
#endif
  }

  Vertex Query(Vertex first, Vertex second) {
    if (IsAncestor(first, second)) {
      return first;
    }
    if (IsAncestor(second, first)) {
      return second;
    }

    Vertex current = first;

    for (int i = ancestors_table_depth_; i >= 0; --i) {
      if (!IsAncestor(ancestors_sparse_table_[current][i], second)) {
        current = ancestors_sparse_table_[current][i];
      }
    }
    return ancestors_sparse_table_[current][0];
  }

  Vertex Query(const std::pair<Vertex, Vertex>& pair) {
    return Query(pair.first, pair.second);
  }

 private:
  void BuildTableDFS(Vertex node, Vertex parent) {
    time_in_[node] = current_time_++;
    ancestors_sparse_table_[node][0] = parent;

    for (int i = 1; i < ancestors_table_depth_; ++i) {
      ancestors_sparse_table_[node][i] =
          ancestors_sparse_table_[ancestors_sparse_table_[node][i - 1]][i - 1];
    }

    for (auto&& successor : tree_[node]) {
      if (successor != parent) {
        BuildTableDFS(successor, node);
      }
    }
    time_out_[node] = current_time_++;
  }

  bool IsAncestor(Vertex left, Vertex right) {
    return time_in_[left] <= time_in_[right] &&
           time_out_[right] <= time_out_[left];
  }

 private:
  size_t num_vertices_;
  size_t ancestors_table_depth_;
  std::vector<std::vector<Vertex>> tree_;
  std::vector<std::vector<Vertex>> ancestors_sparse_table_;
  std::vector<int> time_in_;
  std::vector<int> time_out_;
  int current_time_;
};

using std::cin;
using std::cout;
using std::endl;

class QueryGenerator {
 public:
  QueryGenerator(int64_t a1, int64_t b1, int64_t x, int64_t y, int64_t z,
                 int64_t n)
      : a_(a1), b_(b1), x_(x), y_(y), z_(z), n_(n) {}

  std::pair<Vertex, Vertex> Next(Vertex result) {
    std::pair<Vertex, Vertex> answer((a_ + result) % n_, b_);
    a_ = (a_ * x_ + b_ * y_ + z_) % n_;
    b_ = (b_ * x_ + a_ * y_ + z_) % n_;
    return answer;
  }

 private:
  int64_t a_;
  int64_t b_;
  int64_t x_;
  int64_t y_;
  int64_t z_;
  int64_t n_;
};

int main() {
  size_t num_vertices = 0;
  cin >> num_vertices;
  size_t num_queries = 0;
  cin >> num_queries;
  std::vector<std::vector<Vertex>> tree(num_vertices);
  for (int i = 1; i < num_vertices; ++i) {
    Vertex current;
    cin >> current;
    tree[current].push_back(i);
  }
  LeastCommonAncestorSolver solver(tree);

  int64_t a1 = 0;
  int64_t b1 = 0;
  int64_t x = 0;
  int64_t y = 0;
  int64_t z = 0;

  cin >> a1 >> b1 >> x >> y >> z;

  QueryGenerator generator(a1, b1, x, y, z, num_vertices);
  int64_t sum = 0;
  int64_t result = 0;

  for (int i = 0; i < num_queries; ++i) {
    Vertex result = solver.Query(generator.Next(result));
    sum += result;
  }

  cout << sum << endl;
}