#include <algorithm>
#include <iostream>
#include <stack>
#include <vector>

template <typename T, typename Comparator = std::less<T>>
void InsertionSort(T *array, const int size,
                   Comparator compare = Comparator()) {
  for (int i = 0; i < size; ++i) {
    for (int j = i; j >= 1; --j) {
      if (compare(array[j - 1], array[j])) {
        break;
      }
      std::swap(array[j], array[j - 1]);
    }
  }
}

template <typename T, typename Comparator>
inline T FindMedianOfFive(T *array, Comparator compare) {
  InsertionSort(array, 5, compare);
  return array[2];
}

template <typename T, typename Comparator>
T FindPivot(T *array, const int size, Comparator compare) {
  int five_element_arrays_count = size / 5;
  int tail_length = size % 5;
  T answer = 0;
  if (five_element_arrays_count == 1) {
    return FindMedianOfFive(array, compare);
  } else if (five_element_arrays_count > 1) {
    std::vector<T> medians(five_element_arrays_count);
    for (int i = 0; i < five_element_arrays_count; ++i) {
      medians[i] = FindMedianOfFive(array + (5 * i), compare);
    }
    answer = FindPivot(medians.data(), five_element_arrays_count, compare);
  } else {
    InsertionSort(array, tail_length);
    return array[(tail_length + 1) / 2];
  }
  return answer;
}

template <typename T>
struct Span {
  Span() = default;
  Span(T begin, T end) : begin(begin), end(end) {}

  T begin = -1;
  T end = -1;
};

template <typename T, typename Comparator>
void Partition(T *array, const int size, Span<int> &span, Comparator compare) {
  T pivot = FindPivot(array, size, compare);
  int i = 0;
  int j = 0;
  // p = index of last element equal to pivot
  int p = -1;
  while (j < size) {
    if (array[j] == pivot) {
      ++p;
      if (i == j) {
        std::swap(array[i], array[p]);
      } else {
        std::swap(array[i], array[p]);
        std::swap(array[j], array[p]);
      }
      ++i;
      ++j;
    } else if (compare(pivot, array[j])) {
      ++j;
    } else if (compare(array[j], pivot)) {
      std::swap(array[j], array[i]);
      ++i;
      ++j;
    }
  }
  int right_bound = i;
  int left_bound = i;
  while (p >= 0 && left_bound > 0) {
    --left_bound;
    std::swap(array[p], array[left_bound]);
    --p;
  }
  span.begin = left_bound;
  span.end = right_bound;
  return;
}

template <typename T, typename Comparator>
void QuickSort(T *array, int size, Comparator compare) {
  auto stack = std::stack<std::pair<int, int>>();
  stack.emplace(0, size);
  while (!stack.empty()) {
    int left_bound = stack.top().first;
    int right_bound = stack.top().second;
    stack.pop();
    if (right_bound - left_bound <= 15) {
      InsertionSort(array + left_bound, right_bound - left_bound);
      continue;
    }
    Span<int> pivot_span;
    Partition(array + left_bound, right_bound - left_bound, pivot_span,
              compare);
    pivot_span.begin += left_bound;
    pivot_span.end += left_bound;
    if (pivot_span.begin - left_bound > 1) {
      stack.emplace(left_bound, pivot_span.begin);
    }
    if (right_bound - pivot_span.end - 1 >= 1) {
      stack.emplace(pivot_span.end, right_bound);
    }
  }
  return;
}

template <typename T, typename Comparator = std::less<T>>
void QuickSort(std::vector<T> &array, Comparator compare = Comparator()) {
  QuickSort(array.data(), array.size(), compare);
}

int main() {
  std::ios_base::sync_with_stdio(0);
  std::cin.tie(0);

  auto vector = std::vector<uint32_t>();
  uint32_t value;
  while (std::cin >> value) {
    vector.push_back(value);
  }
  QuickSort(vector);
  for (int i = 9; i < vector.size(); i += 10) {
    std::cout << vector[i] << std::endl;
  }
  return 0;
}
