#include <algorithm>
#include <bitset>
#include <iostream>
#include <vector>

/*
  Дан массив неотрицательных целых 64-разрядных чисел.
  Количество чисел не больше 1000000. Отсортировать массив методом MSD по битам
  (бинарный QuickSort).
*/

template <typename T>
int Partition(T *array, const size_t size, const int bit_depth) {
  int i = 0;
  int j = 0;
  T bit_mask = (1ULL << bit_depth);
  while (j < size) {
    if ((array[j] & bit_mask) != 0) {
      ++j;
    } else {
      std::swap(array[j], array[i]);
      ++i;
      ++j;
    }
  }
  return i;
}

template <typename T>
void BinaryMSDSort(T *array, const int size, const int bit_depth) {
  if (size <= 2) {
    if (size <= 1) {
      return;
    }
    if (array[0] > array[1]) {
      std::swap(array[0], array[1]);
    }
    return;
  }
  int pivot_index = Partition(array, size, bit_depth);
  if (pivot_index > 0) {
    BinaryMSDSort(array, pivot_index, bit_depth - 1);
  }
  if (size - pivot_index > 1) {
    return BinaryMSDSort(array + pivot_index, size - pivot_index,
                         bit_depth - 1);
  }
}

template <typename T>
void BinaryMSDSort(std::vector<T> &array, T max) {
  int bit_depth = 0;
  while (max > 0) {
    max = max >> 1;
    ++bit_depth;
  }
  return BinaryMSDSort(array.data(), array.size(), bit_depth);
}

int main() {
  size_t elements_count = 0;
  std::cin >> elements_count;
  auto array = std::vector<uint64_t>(elements_count);
  uint64_t max = 0;
  for (int i = 0; i < elements_count; ++i) {
    std::cin >> array[i];
    if (array[i] > max) {
      max = array[i];
    }
  }
  BinaryMSDSort(array, max);
  for (int i = 0; i < elements_count; ++i) {
    std::cout << array[i] << std::endl;
  }
  return 0;
}
