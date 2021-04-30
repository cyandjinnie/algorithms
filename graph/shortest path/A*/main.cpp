#include <cassert>
#include <iostream>
#include <queue>
#include <set>
#include <vector>

const int number4 = 4;
const int number5 = 5;
const int number6 = 6;
const int number7 = 7;
const int number8 = 8;
const int number9 = 9;

using Hex = uint8_t;
const uint64_t first_digit_mask = 0xF;
const uint64_t final_arrangement = 0x123456780;

struct Point2D {
  Point2D(int x, int y) : x(x), y(y) {}

  static int Distance(const Point2D& lhs, const Point2D& rhs) {
    return abs(lhs.x - rhs.x) + abs(lhs.y - rhs.y);
  }

  int x;
  int y;
};

enum class Action { LEFT, RIGHT, UP, DOWN };

char ActionToChar(Action action) {
  switch (action) {
    case Action::LEFT:
      return 'L';
      break;
    case Action::RIGHT:
      return 'R';
      break;
    case Action::UP:
      return 'U';
      break;
    case Action::DOWN:
      return 'D';
      break;
    default:
      assert(false);
  }
}

class BoardArrangement {
 public:
  explicit BoardArrangement(std::vector<uint8_t>& vector) {
    assert(vector.size() == number9);
    bitset_ = 0;
    for (uint8_t i = 0; i < number9; ++i) {
      SetAtPos(i, vector[i]);
      if (vector[i] == 0) {
        null_pos_ = i;
      }
    }
    actions_ = 0;
  }

  BoardArrangement() {
    for (uint8_t i = 0; i < number9; ++i) {
      SetAtPos(i, i);
    }
    bitset_ = final_arrangement;
    null_pos_ = number8;
    actions_ = 0;
  }

  BoardArrangement(const BoardArrangement& other) = default;

  BoardArrangement(const BoardArrangement& last, Action action)
      : bitset_(last.bitset_),
        null_pos_(last.null_pos_),
        actions_(last.actions_ + 1) {
    int d_coord = 0;
    switch (action) {
      case Action::LEFT:
        assert(null_pos_ % 3 > 0);
        d_coord = -1;
        break;
      case Action::RIGHT:
        assert(null_pos_ % 3 < 2);
        d_coord = 1;
        break;
      case Action::UP:
        assert(null_pos_ / 3 > 0);
        d_coord = -3;
        break;
      case Action::DOWN:
        assert(null_pos_ / 3 < 2);
        d_coord = 3;
        break;
      default:
        break;
    }
    auto buffer = GetAtPos(null_pos_ + d_coord);
    SetAtPos(null_pos_, buffer);
    SetAtPos(null_pos_ + d_coord, 0);
    null_pos_ = null_pos_ + d_coord;
  }

  uint8_t GetNullPos() const { return null_pos_; }

  uint64_t GetBitset() const { return bitset_; }

  void SetAtPos(uint8_t index, uint8_t value) {
    assert(0 <= index && index < number9);
    assert(0 <= value && value < number9);
    bitset_ &= ~(first_digit_mask << number4 * (number8 - index));
    bitset_ |= (first_digit_mask & value) << number4 * (number8 - index);
  }

  uint8_t GetAtPos(uint8_t index) const {
    assert(0 <= index && index < number9);
    return (bitset_ >> (number4 * (number8 - index))) & first_digit_mask;
  }

  int Heuristic() const { return HScore() + GScore(); }

  int HScore() const { return static_cast<int>(FindManhattanDistance()); }

  int GScore() const { return actions_; }

  bool IsActionPossible(Action action) const {
    bool answer = false;
    switch (action) {
      case Action::LEFT:
        if (null_pos_ % 3 > 0) {
          answer = true;
        }
        break;
      case Action::RIGHT:
        if (null_pos_ % 3 < 2) {
          answer = true;
        }
        break;
      case Action::UP:
        if (null_pos_ / 3 > 0) {
          answer = true;
        }
        break;
      case Action::DOWN:
        if (null_pos_ / 3 < 2) {
          answer = true;
        }
        break;
      default:
        break;
    }
    return answer;
  }

  enum class Parity { EVEN, ODD };

  Parity FindParityInvariant() const {
    uint64_t counter = 0;
    for (uint8_t i = 0; i < number9; ++i) {
      if (GetAtPos(i) != 0) {
        for (uint8_t j = 0; j < i; ++j) {
          if (GetAtPos(j) > GetAtPos(i)) {
            ++counter;
          }
        }
      }
    }

    return counter % 2 == 0 ? Parity::EVEN : Parity::ODD;
  }

  uint64_t FindManhattanDistance() const {
    uint64_t distance = 0;
    for (uint8_t i = 0; i < number9; ++i) {
      if (i != null_pos_) {
        auto target = Point2D((GetAtPos(i) - 1) % 3, (GetAtPos(i) - 1) / 3);
        auto current = Point2D(i % 3, i / 3);
        auto temp = Point2D::Distance(target, current);
        distance += temp;
      } else {
        auto target = Point2D(2, 2);
        auto current = Point2D(i % 3, i / 3);
        auto temp = Point2D::Distance(target, current);
        distance += temp;
      }
    }

    return distance;
  }

  bool operator==(const BoardArrangement& rhs) {
    return rhs.bitset_ == bitset_;
  }

  bool IsFinal() const { return bitset_ == final_arrangement; }

 private:
  int actions_;
  uint64_t bitset_;
  uint8_t null_pos_;
};

bool operator<(const BoardArrangement& lhs, const BoardArrangement& rhs) {
  return (lhs.Heuristic() == rhs.Heuristic()
              ? lhs.GetBitset() > rhs.GetBitset()
              : lhs.Heuristic() > rhs.Heuristic());
}

struct Path {
  Path(const BoardArrangement& initial) : last(initial) {}

  Path(const Path& path, Action action)
      : actions(path.actions), last(BoardArrangement(path.last, action)) {
    actions.push_back(action);
  }

  std::vector<Action> actions;
  BoardArrangement last;
};

bool operator<(const Path& lhs, const Path& rhs) { return lhs.last < rhs.last; }

class TagGameSolver {
 public:
  TagGameSolver() = default;

  int FindMinPath(const BoardArrangement& initial, std::string& output_path) {
    closed_ = std::set<BoardArrangement>();
    open_ = std::priority_queue<Path>();
    open_.emplace(initial);
    while (!open_.empty()) {
      auto path = open_.top();
      auto current = path.last;
      open_.pop();
      if (closed_.find(current) != closed_.end()) {
        continue;
      }
      if (current.IsFinal()) {
        GetAnswer(path, output_path);
        // Output min_path size
        return current.GScore();
      }
      AddAdjacentArrangements(path);
      closed_.insert(current);
    }
    std::cout << "FAIL" << std::endl;
  }

  void GetAnswer(Path& path, std::string& output_path) {
    output_path = "";
    for (auto action : path.actions) {
      output_path.push_back(ActionToChar(action));
    }
  }

  void AddAdjacentArrangements(Path& path) {
    auto current = path.last;
    if (current.IsActionPossible(Action::UP)) {
      open_.emplace(path, Action::UP);
    }
    if (current.IsActionPossible(Action::DOWN)) {
      open_.emplace(path, Action::DOWN);
    }
    if (current.IsActionPossible(Action::LEFT)) {
      open_.emplace(path, Action::LEFT);
    }
    if (current.IsActionPossible(Action::RIGHT)) {
      open_.emplace(path, Action::RIGHT);
    }
  }

 private:
  std::set<BoardArrangement> closed_;
  std::priority_queue<Path> open_;
};

int main() {
  std::vector<uint8_t> first_configuration;
  for (int i = 0; i < number9; ++i) {
    int digit = 0;
    std::cin >> digit;
    first_configuration.emplace_back(digit);
  }

  BoardArrangement arrangement(first_configuration);

  if (arrangement.FindParityInvariant() == BoardArrangement::Parity::ODD) {
    std::cout << -1;
  } else {
    TagGameSolver solver;
    std::string actions;
    std::cout << solver.FindMinPath(arrangement, actions) << std::endl;
    std::cout << actions << std::endl;
  }
}
