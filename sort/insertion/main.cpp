#include <functional>
#include <iostream>
#include <vector>

template <typename IteratorType,
          typename Comparator = std::less<typename IteratorType::value_type> >
void InsertionSort(IteratorType begin, IteratorType end,
                   Comparator compare = Comparator()) {
  for (auto i = std::next(begin); i != end; i = std::next(i)) {
    for (auto j = i; j != begin && compare(*j, *std::prev(j));
         j = std::prev(j)) {
      std::swap(*j, *std::prev(j));
    }
  }
}

struct Crate {
 public:
  int x = 0;
  int y = 0;
  int z = 0;
  int index = -1;

  Crate() = default;
  Crate(const int _x, const int _y, const int _z, const int _index)
      : index(_index) {
    std::vector<int> array = {_x, _y, _z};
    InsertionSort(array.begin(), array.end());
    x = array[0];
    y = array[1];
    z = array[2];
  }

  const Crate& operator=(const Crate& rhs) {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    index = rhs.index;
    return rhs;
  }

  static bool Compare(const Crate& lhs, const Crate& rhs) {
    return lhs.z < rhs.z && lhs.x < rhs.x && lhs.y < rhs.y;
  }
};

int main() {
  int crates_count = 0;
  std::cin >> crates_count;
  std::vector<Crate> crates;
  crates.reserve(crates_count);
  for (int i = 0; i < crates_count; ++i) {
    int x = 0;
    int y = 0;
    int z = 0;
    std::cin >> x >> y >> z;
    crates.emplace_back(x, y, z, i);
  }
  InsertionSort(crates.begin(), crates.end(), Crate::Compare);
  for (int i = 0; i < crates.size(); ++i) {
    std::cout << crates[i].index << " ";
  }
  std::cout << std::endl;
  return 0;
}
