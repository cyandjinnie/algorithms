#include <iostream>
#include <memory>
#include <stack>
#include <tuple>

/*
  Дано число N < 106 и последовательность пар целых чисел из [-231, 231]
  длиной N. Построить декартово дерево из N узлов, характеризующихся парами
  чисел (Xi, Yi). Каждая пара чисел (Xi, Yi) определяет ключ Xi и приоритет Yi
  в декартовом дереве.
  Добавление узла в декартово дерево выполняйте следующим образом: При
  добавлении узла (x, y) выполняйте спуск по ключу до узла P с меньшим
  приоритетом. Затем разбейте найденное поддерево по ключу x так, чтобы в
  первом поддереве все ключи меньше x, а во втором больше или равны x.
  Получившиеся два дерева сделайте дочерними для нового узла (x, y). Новый узел
  вставьте на место узла P.
  Построить также наивное дерево поиска по ключам Xi.
  Т.е., при добавлении очередного числа K в наивное дерево с корнем root,
  если root→Key ≤ K, то узел K добавляется в правое поддерево root; иначе в
  левое поддерево root.
  Вычислить разницу глубин наивного дерева поиска и декартового дерева.
  Разница может быть отрицательна, необходимо вывести модуль разности.
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

template <typename T>
class Treap : public BinarySearchTree<T> {
  using typename BinarySearchTree<T>::ChildType;
  using typename BinarySearchTree<T>::Node;
  using BinarySearchTree<T>::GetChild;
  using BinarySearchTree<T>::Locate;
  using BinarySearchTree<T>::root_;

 public:
  Treap() = default;

  void Insert(const T& key, int priority) { Insert(root_, key, priority); }

  void Insert(const T& key) override { Insert(key, rand() % max_random_value); }

  void Erase(const T& key) override {
    auto [node, type] = Locate(key);
    auto target_node = GetChild(node, type);
    target_node =
        Merge(std::static_pointer_cast<TreapNode>(target_node->left_child),
              std::static_pointer_cast<TreapNode>(target_node->right_child));
  }

 private:
  struct TreapNode : Node {
    TreapNode(const T& key_init, const int priority_init)
        : Node(key_init), priority(priority_init) {}

    int priority;
  };

  using TreapNodePtr = shared_ptr<TreapNode>;
  using typename BinarySearchTree<T>::NodePtr;

  std::tuple<NodePtr, NodePtr> Split(NodePtr treap, const T& key) {
    if (treap == nullptr) {
      return {nullptr, nullptr};
    }
    if (key > treap->key) {
      auto [treap1, treap2] = Split(treap->right_child, key);
      treap->right_child = treap1;
      return {treap, treap2};
    } else {
      auto [treap1, treap2] = Split(treap->left_child, key);
      treap->left_child = treap2;
      return {treap1, treap};
    }
  }

  NodePtr Merge(TreapNodePtr treap1, TreapNodePtr treap2) {
    if (treap1 == nullptr) {
      return treap2;
    }
    if (treap2 == nullptr) {
      return treap1;
    }
    if (treap1->priority > treap2->priority) {
      treap1->right_child =
          Merge(std::static_pointer_cast<TreapNode>(treap1->right_child),
                std::static_pointer_cast<TreapNode>(treap2));
      return treap1;
    } else {
      treap2->left_child =
          Merge(std::static_pointer_cast<TreapNode>(treap1),
                std::static_pointer_cast<TreapNode>(treap2->left_child));
      return treap2;
    }
  }

  void Allocate(NodePtr& node, const T& value, int priority) {
    node = std::make_shared<TreapNode>(value, priority);
  }

  void Insert(NodePtr& node, const T& key, int priority) {
    if (node == nullptr) {
      Allocate(node, key, priority);
      return;
    }
    if (std::static_pointer_cast<TreapNode>(node)->priority > priority) {
      if (key < node->key) {
        Insert(node->left_child, key, priority);
      } else {
        Insert(node->right_child, key, priority);
      }
    } else {
      auto [treap1, treap2] = Split(node, key);
      node = nullptr;
      Allocate(node, key, priority);
      node->left_child = treap1;
      node->right_child = treap2;
    }
  }

 private:
  const int max_random_value = 100000;
};

int main() {
  auto binary_tree = BinarySearchTree<int>();
  auto treap = Treap<int>();
  int elements_count = 0;
  std::cin >> elements_count;
  for (int i = 0; i < elements_count; ++i) {
    int value = 0;
    int priority = 0;
    std::cin >> value;
    std::cin >> priority;
    binary_tree.Insert(value);
    treap.Insert(value, priority);
  }
  std::cout << abs(binary_tree.CountDepth() - treap.CountDepth());
}
