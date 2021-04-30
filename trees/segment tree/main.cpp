#include <cassert>
#include <iostream>
#include <vector>

const int inf = std::numeric_limits<int>::max();

template <typename Type>
struct Node {
  Node() = default;

  Node(Type value)
      : sum(value), prefix_sum(value), suffix_sum(value), answer(value) {}

  Node(const Node& left_child, const Node& right_child)
      : sum(left_child.sum + right_child.sum),
        prefix_sum(std::max(left_child.prefix_sum,
                            left_child.sum + right_child.prefix_sum)),
        suffix_sum(std::max(right_child.suffix_sum,
                            right_child.sum + left_child.suffix_sum)),
        answer(std::max(std::max(left_child.answer, right_child.answer),
                        left_child.suffix_sum + right_child.prefix_sum)) {}

  bool is_neutral;
  Type sum;
  Type prefix_sum;
  Type suffix_sum;
  Type answer;
};

const int number_four = 4;

template <typename Type>
class SegmentTree {
 public:
  explicit SegmentTree(const std::vector<Type>& array)
      : nodes_(number_four * array.size()), num_elements_(array.size()) {
    Build(array, 1, 0, array.size() - 1);
  }

  Node<Type> Query(int left_bound, int right_bound) {
    return QueryRecursive(1, 0, num_elements_ - 1, left_bound - 1,
                          right_bound - 1);
  }

 private:
  Node<Type> QueryRecursive(int node, int tl, int tr, int l, int r) {
    if (l == tl && tr == r) {
      return nodes_[node];
      assert(node < num_elements_);
    }
    int middle_cut = (tl + tr) / 2;
    if (r <= middle_cut) {
      return QueryRecursive(node * 2, tl, middle_cut, l, r);
    }
    if (l > middle_cut) {
      return QueryRecursive(node * 2 + 1, middle_cut + 1, tr, l, r);
    }
    return Node<Type>(
        QueryRecursive(node * 2, tl, middle_cut, l, middle_cut),
        QueryRecursive(node * 2 + 1, middle_cut + 1, tr, middle_cut + 1, r));
  }

  void Build(const std::vector<Type>& array, int node, int tl, int tr) {
    if (tl == tr) {
      nodes_[node] = array[tl];
    } else {
      int middle_cut = (tl + tr) / 2;
      Build(array, node * 2, tl, middle_cut);
      Build(array, node * 2 + 1, middle_cut + 1, tr);
      nodes_[node] = Node<Type>(nodes_[node * 2], nodes_[node * 2 + 1]);
    }
  }

 private:
  std::vector<Node<Type>> nodes_;
  size_t num_elements_;
};

int main() {
  size_t num_elements = 0;
  while (std::cin >> num_elements) {
    size_t num_queries = 0;
    std::cin >> num_queries;

    std::vector<int> array(num_elements);
    for (int i = 0; i < num_elements; ++i) {
      int value = 0;
      std::cin >> value;
      array[i] = value;
    }
    SegmentTree<int> tree(array);

    for (int i = 0; i < num_queries; ++i) {
      int left_bound = 0;
      int right_bound = 0;
      std::cin >> left_bound >> right_bound;
      std::cout << tree.Query(left_bound, right_bound).answer << std::endl;
    }
  }
}