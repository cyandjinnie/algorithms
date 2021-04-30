#include <iostream>
#include <memory>
#include <stack>
#include <tuple>

/*
  Дано число N ≤ 104 и последовательность целых чисел из [-231..231] длиной N.
  Требуется построить бинарное дерево, заданное наивным порядком вставки. Т.е.,
  при добавлении очередного числа K в дерево с корнем root, если root→Key ≤ K,
  то узел K добавляется в правое поддерево root; иначе в левое поддерево root.
  Выведите элементы в порядке post-order (снизу вверх).
  Рекурсия запрещена.
*/

using std::shared_ptr;
using std::tuple;

template <typename T>
class Set {
 public:
  virtual ~Set() = default;
  virtual void Insert(const T&) = 0;
  virtual bool HasKey(const T&) const = 0;
  virtual void Erase(const T&) = 0;
};

template <typename T>
class BinarySearchTree : public Set<T> {
 public:
  BinarySearchTree() = default;

  void Insert(const T& new_key) override {
    if (root_ == nullptr) {
      root_ = std::make_shared<Node>(new_key);
      return;
    }
    auto [node, type] = Locate(new_key);
    Allocate(node, type, new_key);
  }

  int CountDepth() const { return CountDepth(root_); }

  bool HasKey(const T& key) const override {
    auto [node, type] = Locate(key);
    return node != nullptr;
  }

  void Erase(const T& key) override {
    auto [node, type] = Locate(key);
    if (node != nullptr) {
      EraseChildAndRelink(node, type);
    }
  }

  void PrintPostOrder() const {
    NodePtr current = nullptr;
    std::stack<NodePtr> nodes_stack;
    std::stack<T> output_stack;
    nodes_stack.emplace(root_);
    while (!nodes_stack.empty()) {
      current = nodes_stack.top();
      nodes_stack.pop();
      if (current == nullptr) {
        continue;
      }
      nodes_stack.emplace(current->left_child);
      nodes_stack.emplace(current->right_child);
      output_stack.emplace(current->key);
    }
    int count = output_stack.size();
    for (int i = 0; i < count; ++i) {
      std::cout << output_stack.top() << " ";
      output_stack.pop();
    }
    std::cout << std::endl;
  }

 protected:
  enum ChildType { Undefined, Root, LeftChild, RightChild };

  struct Node {
    explicit Node(T key_init) : key(key_init) {}
    virtual ~Node() = default;

    T key;
    shared_ptr<Node> left_child = nullptr;
    shared_ptr<Node> right_child = nullptr;
  };

  using NodePtr = shared_ptr<Node>;

  tuple<NodePtr, ChildType> Locate(const T& key) const {
    NodePtr current = root_;
    if (key == root_->key) {
      return {current, ChildType::Root};
    }
    while (true) {
      if (key < current->key) {
        if (current->left_child == nullptr || key == current->left_child->key) {
          return {current, ChildType::LeftChild};
        }
        current = current->left_child;
      } else if (key > current->key) {
        if (current->right_child == nullptr ||
            key == current->right_child->key) {
          return {current, ChildType::RightChild};
        }
        current = current->right_child;
      }
    }
  }

  int CountDepth(const NodePtr& node) const {
    if (node == nullptr) {
      return 0;
    }
    return std::max(CountDepth(node->left_child),
                    CountDepth(node->right_child)) +
           1;
  }

  NodePtr GetChild(NodePtr parent, ChildType type) {
    switch (type) {
      case Root:
        return parent;
      case LeftChild:
        return parent->left_child;
      case RightChild:
        return parent->right_child;
      default:
        return nullptr;
    }
  }

  void EraseChildAndRelink(NodePtr& node, ChildType type) {
    NodePtr target_node = GetChild(node, type);
    if (target_node->left_child == nullptr &&
        target_node->right_child == nullptr) {
      target_node = nullptr;
    } else if (target_node->right_child == nullptr &&
               target_node->left_child != nullptr) {
      target_node = target_node->left_child;
    } else if (target_node->right_child != nullptr &&
               target_node->left_child == nullptr) {
      target_node = target_node->right_child;
    } else {
      NodePtr parent_to_successor = node;
      while (parent_to_successor->left_child->left_child != nullptr) {
        parent_to_successor = parent_to_successor->left_child->left_child;
      }
      node->key = parent_to_successor->left_child->key;
      parent_to_successor->left_child = nullptr;
    }
  }

 private:
  void Allocate(NodePtr& parent, ChildType type, const T& key) {
    switch (type) {
      case ChildType::LeftChild: {
        if (parent->left_child == nullptr) {
          parent->left_child = std::make_shared<Node>(key);
        }
        break;
      }
      case ChildType::RightChild: {
        if (parent->right_child == nullptr) {
          parent->right_child = std::make_shared<Node>(key);
        }
        break;
      }
      default: { return; }
    }
  }

 protected:
  NodePtr root_ = nullptr;
};

int main() {
  int nodes_count = 0;
  std::cin >> nodes_count;
  auto tree = BinarySearchTree<int>();
  for (int i = 0; i < nodes_count; ++i) {
    int value = 0;
    std::cin >> value;
    tree.Insert(value);
  }
  tree.PrintPostOrder();
  return 0;
}
