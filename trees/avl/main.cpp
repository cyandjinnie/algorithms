#include <iostream>
#include <memory>
#include <stack>
#include <tuple>

/*
  В одной военной части решили построить в одну шеренгу по росту. Т.к. часть
  была далеко не образцовая, то солдаты часто приходили не вовремя, а то их и
  вовсе приходилось выгонять из шеренги за плохо начищенные сапоги. Однако
  солдаты в процессе прихода и ухода должны были всегда быть выстроены по росту
  – сначала самые высокие, а в конце – самые низкие. За расстановку солдат
  отвечал прапорщик, который заметил интересную особенность – все солдаты в
  части разного роста. Ваша задача состоит в том, чтобы помочь прапорщику
  правильно расставлять солдат, а именно для каждого приходящего солдата
  указывать, перед каким солдатом в строе он должен становится. Требуемая
  скорость выполнения команды - O(log n).
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
class AVLTree : public BinarySearchTree<T> {
  using typename BinarySearchTree<T>::Node;
  using typename BinarySearchTree<T>::ChildType;
  using typename BinarySearchTree<T>::NodePtr;
  using BinarySearchTree<T>::GetChild;
  using BinarySearchTree<T>::root_;

 public:
  AVLTree() = default;

  int InsertWithIndex(const T& key) {
    int position = 0;
    Insert(root_, key, position);
    return position;
  }

  void Insert(const T& key) override {
    int position = 0;
    Insert(root_, key, position);
  }

  void Erase(const T& key) override { return Erase(root_, key); }

  void EraseByIndex(int index) { Erase(FindOrderStatistic(root_, index)); }

 private:
  struct AVLNode : Node {
    AVLNode(const T& key) : Node(key) {}

    unsigned short height = 1;
    int count = 1;
  };
  using AVLNodePtr = shared_ptr<AVLNode>;

  int Height(NodePtr node) {
    return nullptr == node ? 0
                           : std::static_pointer_cast<AVLNode>(node)->height;
  }

  T FindOrderStatistic(NodePtr node, int index) {
    int right_count = Count(node->right_child);
    if (index < right_count) {
      return FindOrderStatistic(node->right_child, index);
    } else if (index > right_count) {
      return FindOrderStatistic(node->left_child, index - right_count - 1);
    } else {
      // index == right_count
      return node->key;
    }
  }

  void FixHeights(NodePtr node) {
    if (node != nullptr) {
      std::static_pointer_cast<AVLNode>(node)->height =
          std::max(Height(node->right_child), Height(node->left_child)) + 1;
      std::static_pointer_cast<AVLNode>(node)->count =
          Count(node->left_child) + Count(node->right_child) + 1;
    }
  }

  int Count(NodePtr node) {
    return nullptr == node ? 0 : std::static_pointer_cast<AVLNode>(node)->count;
  }

  int Difference(NodePtr node) {
    return Height(node->right_child) - Height(node->left_child);
  }

  void Balance(NodePtr& node) {
    FixHeights(node);
    if (node == nullptr) {
      return;
    }
    if (Difference(node) == 2) {
      if (Difference(node->right_child) < 0) {
        RotateRight(node->right_child);
      }
      RotateLeft(node);
    } else if (Difference(node) == -2) {
      if (Difference(node->left_child) > 0) {
        RotateLeft(node->left_child);
      }
      RotateRight(node);
    }
  }

  void Allocate(NodePtr& parent, const T& key) {
    parent = std::make_shared<AVLNode>(key);
  }

  void Insert(NodePtr& node, const T& key, int& position) {
    if (nullptr == node) {
      Allocate(node, key);
      FixHeights(node);
    } else {
      if (key < node->key) {
        position += Count(node->right_child) + 1;
        Insert(node->left_child, key, position);
      } else {
        Insert(node->right_child, key, position);
      }
      Balance(node);
    }
  }

  T FindSucceedingValue(NodePtr& node) {
    if (node->left_child != nullptr) {
      return FindSucceedingValue(node->left_child);
    } else {
      return node->key;
    }
  }

  void EraseSucceedingElement(NodePtr& node) {
    if (node->left_child == nullptr) {
      node = node->right_child;
    } else {
      EraseSucceedingElement(node->left_child);
      Balance(node);
    }
  }

  void Erase(NodePtr& node, const T& key) {
    if (nullptr != node) {
      if (key < node->key) {
        Erase(node->left_child, key);
      } else if (key > node->key) {
        Erase(node->right_child, key);
      } else {
        auto left_subtree = node->left_child;
        auto right_subtree = node->right_child;
        if (nullptr == right_subtree) {
          node = left_subtree;
        } else {
          T succeeding_key = FindSucceedingValue(right_subtree);
          node->key = succeeding_key;
          EraseSucceedingElement(node->right_child);
        }
      }
      Balance(node);
    }
  }

  void RotateLeft(NodePtr& node_a) {
    NodePtr node_b = node_a->right_child;
    node_a->right_child = node_b->left_child;
    node_b->left_child = node_a;
    FixHeights(node_a);
    FixHeights(node_b);
    node_a = node_b;
  }

  void RotateRight(NodePtr& node_a) {
    NodePtr node_b = node_a->left_child;
    node_a->left_child = node_b->right_child;
    node_b->right_child = node_a;
    FixHeights(node_a);
    FixHeights(node_b);
    node_a = node_b;
  }
};

using std::cin;
using std::cout;
using std::endl;

int main() {
  enum CommandType { Undefined = 0, Insert = 1, Erase = 2 };
  int commands_count = 0;
  cin >> commands_count;
  auto tree = AVLTree<int>();
  for (int i = 0; i < commands_count; ++i) {
    int key = 0;
    int command = 0;
    cin >> command >> key;
    switch (command) {
      case Insert: {
        cout << tree.InsertWithIndex(key) << endl;
        break;
      }
      case Erase: {
        tree.EraseByIndex(key);
        break;
      }
      default:
        break;
    }
  }
}
