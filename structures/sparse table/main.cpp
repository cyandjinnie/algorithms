#include <math.h>
#include <algorithm>
#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::endl;

const int inf = std::numeric_limits<int>::max();

//#define DEBUG

template <typename Type>
class TwoStatSparseTable {
 public:
  explicit TwoStatSparseTable(const std::vector<Type>& vec) {
    // Precalc

    // floor logarithm array
    // log(0) = -inf
    log_floor_.push_back(-inf);
    for (int i = 1; i <= vec.size(); ++i) {
      log_floor_.push_back(log2(i));
    }

    // Initializing 1-element array
    table_.push_back(std::vector<std::pair<Type, Type>>(vec.size()));
    for (int i = 0; i < vec.size(); ++i) {
      table_[0][i] = std::make_pair(vec[i], inf);
    }

    // Recursive table calculation
    size_t current_length = vec.size();
    for (int i = 1; i <= floor(log2(vec.size())); ++i) {
      current_length -= round(pow(2, i - 1));
      table_.emplace_back(std::vector<std::pair<Type, Type>>(current_length));
      for (int j = 0; j < current_length; ++j) {
        table_[i][j] =
            MinPair(table_[i - 1][j], table_[i - 1][j + pow(2, i - 1)]);
      }
    }

// debug output
#ifdef DEBUG
    for (auto&& row : table_) {
      for (auto&& elem : row) {
        cout << "(" << elem.first << ", " << elem.second << ")";
      }
      cout << endl;
    }
#endif
  }

  Type Query(int left, int right) {
    int j = log_floor_[right - left];

    return MinPair(table_[j][left], table_[j][right - pow(2, j)]).second;
  }

 private:
  std::pair<Type, Type> MinPair(std::pair<Type, Type> left,
                                std::pair<Type, Type> right) {
    std::pair<Type, Type> answer;

    answer.first =
        std::min({left.first, left.second, right.first, right.second});
    answer.second = inf;
    bool found_flag = false;
    for (auto current : {left.first, left.second, right.first, right.second}) {
      if (current > answer.first) {
        answer.second = std::min(answer.second, current);
        found_flag = true;
      }
    }

    if (!found_flag) {
      answer.second = answer.first;
    }

    return answer;
  }

 private:
  std::vector<int> log_floor_;
  std::vector<std::vector<std::pair<Type, Type>>> table_;
};

int main() {
  int numbers_cnt = 0;
  std::cin >> numbers_cnt;
  int spans_cnt = 0;
  std::cin >> spans_cnt;
  std::vector<int> array;
  for (int i = 0; i < numbers_cnt; ++i) {
    int temp = 0;
    cin >> temp;
    array.push_back(temp);
  }
  TwoStatSparseTable<int> table(array);
  for (int i = 0; i < spans_cnt; ++i) {
    int left = 0;
    int right = 0;
    cin >> left >> right;
    cout << table.Query(left - 1, right) << endl;
  }
}