#pragma GCC optimize("Ofast,unroll-all-loops")

#include <iostream>
#include <memory>
#include <vector>

const int alphabet_size = 26;

//#define DEBUG

class Trie {
 public:
  explicit Trie(const std::string& pattern);

  void AddString(const std::string& string, int pattern_index);

  inline std::vector<int> ProcessText(const std::string& input);

 private:
  struct Node {
    Node(int parent, char char_to_parent)
        : parent(parent), char_to_parent(char_to_parent) {
      is_terminal = false;
      suffix_link = -1;
      terminal_link = -1;

      for (int i = 0; i < alphabet_size; ++i) {
        son[i] = -1;
        next[i] = -1;
      }
    }

    bool is_terminal;
    int son[alphabet_size];
    int next[alphabet_size];
    int parent;
    int suffix_link;
    int terminal_link;
    char char_to_parent;
    std::vector<int> pattern_indices;
  };

  void ParseWildcardPattern(const std::string& pattern);

  char GetCharIndex(char c) { return c - 'a'; }

  /// Check if son of ith node passing with char c exists
  /// @param      i - index of the nod??k????r???e
  /// @param      c - char we proceed with
  bool SonExists(int i, char c) { return nodes_[i].son[c] >= 0; }

  /// Get index of the next succeeding node passing with char c
  /// @param      i - index of the node
  /// @param      c - char we proceed with
  int GetLink(int i, char c);

  /// Get suffix link of node i
  /// @param      i - node index
  /// @return     int - suffix link of the correspoding node
  int GetSuffixLink(int i);

  int GetTerminalLink(int i);

 private:
  const int root_ = 0;
  const int pattern_length_;

  std::vector<Node> nodes_;
  std::vector<std::pair<int, std::string>> subpattern_offset_;
};

///////////////////////////////////////////////////////////////
/// Trie implementation ///////////////////////////////////////
///////////////////////////////////////////////////////////////

Trie::Trie(const std::string& pattern)
  : nodes_(1, Node(0, -1)), pattern_length_(pattern.length()) {
  nodes_[root_].suffix_link = root_;

  ParseWildcardPattern(pattern);
}

void Trie::AddString(const std::string& string, int pattern_index) {
  int current_node = root_;
  for (int i = 0; i < string.length(); ++i) {
    char char_index = string[i] - 'a';
    if (!SonExists(current_node, char_index)) {
      nodes_.emplace_back(Node(current_node, char_index));
      nodes_[current_node].son[char_index] = nodes_.size() - 1;
    }
    current_node = nodes_[current_node].son[char_index];
  }
  nodes_[current_node].is_terminal = true;
  nodes_[current_node].pattern_indices.push_back(pattern_index);
}

inline std::vector<int> Trie::ProcessText(const std::string& input) {
  // Index of the current node
  int current_node = root_;
  // Iterating with i
  int i = 0;
  std::vector<int> subpattern_entry_count(input.length(), 0);

  while (i < input.length()) {
    char cur_char = input[i];
    current_node = GetLink(current_node, GetCharIndex(cur_char));

    // A temporary node index which helps us pass through terminal suffix
    // links
    int u = current_node;
    do {
      // If a node is terminal, write information to the
      // subpattern_entry_count array
      if (nodes_[u].is_terminal) {
        for (auto index : nodes_[u].pattern_indices) {
          auto&& [offset, pattern] = subpattern_offset_[index];
          int resulting_target_index = i - offset - pattern.length() + 1;
          if (resulting_target_index >= 0) {
            subpattern_entry_count[resulting_target_index]++;
          }
        }
      }
      u = GetTerminalLink(u);
    } while (u != root_);
    ++i;
  }

#ifdef DEBUG
  std::cout << "[ ";
    for (auto subpattern_entry_count : subpattern_entry_count) {
      std::cout << subpattern_entry_count << " ";
    }
    std::cout << "]" << std::endl;
#endif  // DEBUG

  std::vector<int> result;

  for (int j = 0; j < subpattern_entry_count.size(); ++j) {
    if (subpattern_entry_count[j] == subpattern_offset_.size() &&
        (j + pattern_length_) <= subpattern_entry_count.size()) {
      result.push_back(j);
    }
  }

  return result;
}

void Trie::ParseWildcardPattern(const std::string& pattern) {
  std::string current_subpattern;
  bool reading_subpattern = false;

  for (int i = 0; i < pattern.length(); ++i) {
    char current_symbol = pattern[i];
    if (current_symbol == '?') {
      if (reading_subpattern) {
        subpattern_offset_.emplace_back(std::make_pair(
          i - current_subpattern.length(), current_subpattern));
        reading_subpattern = false;
      }
      continue;
    } else {
      if (!reading_subpattern) {
        current_subpattern = "";
        reading_subpattern = true;
      }
      current_subpattern += current_symbol;
    }
  }

  if (reading_subpattern) {
    subpattern_offset_.emplace_back(std::make_pair(
      pattern_length_ - current_subpattern.length(), current_subpattern));
  }

#ifdef DEBUG
  for (auto [offset, pattern] : subpattern_offset_) {
      std::cout << pattern << "\t:: " << offset << std::endl;
    }
#endif  // DEBUG

  for (int i = 0; i < subpattern_offset_.size(); ++i) {
    AddString(subpattern_offset_[i].second, i);
  }
}

int Trie::GetLink(int i, char c) {
  if (nodes_[i].next[c] == -1) {
    if (SonExists(i, c)) {
      nodes_[i].next[c] = nodes_[i].son[c];
    } else if (i == root_) {
      nodes_[i].next[c] = root_;
    } else {
      nodes_[i].next[c] = GetLink(GetSuffixLink(i), c);
    }
  }
  return nodes_[i].next[c];
}

int Trie::GetSuffixLink(int i) {
  if (nodes_[i].suffix_link == -1) {
    if (i == root_ || nodes_[i].parent == root_) {
      nodes_[i].suffix_link = 0;
    } else {
      nodes_[i].suffix_link =
        GetLink(GetSuffixLink(nodes_[i].parent), nodes_[i].char_to_parent);
    }
  }

  return nodes_[i].suffix_link;
}

int Trie::GetTerminalLink(int i) {
  if (nodes_[i].terminal_link == -1) {
    if (nodes_[GetSuffixLink(i)].is_terminal) {
      nodes_[i].terminal_link = GetSuffixLink(i);
    } else if (GetSuffixLink(i) == root_) {
      nodes_[i].terminal_link = root_;
    } else {
      nodes_[i].terminal_link = GetTerminalLink(GetSuffixLink(i));
    }
  }

  return nodes_[i].terminal_link;
}

///////////////////////////////////////////////////////////////

int main() {
  std::cin.tie(nullptr);
  std::ios::sync_with_stdio(false);

  std::string pattern, text;

  std::cin >> pattern;
  std::cin >> text;

  Trie trie(pattern);
  std::vector<int> matches = trie.ProcessText(text);

  for (int i = 0; i < matches.size(); ++i) {
    std::cout << matches[i] << ' ';
  }

  return 0;
}