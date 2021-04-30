#include <memory.h>
#include <stdint.h>
#include <iostream>
#include <vector>

/*
  Дана последовательность целых чисел из диапазона (-1000000000 .. 1000000000).
  Длина последовательности не больше 1000000. Числа записаны по одному в строке.
  Количество чисел не указано. Пусть количество элементов n, и числа записаны
  в массиве a = a[i]: i из [0..n-1].  Требуется напечатать количество таких пар
  индексов (i,j) из [0..n-1], что (i < j и a[i] > a[j])
*/

template <class T>
void Merge(T* array1, size_t size1, T* array2, size_t size2, T* destination,
           int64_t& counter) {
  int i = 0;
  int j = 0;
  int swaps_done = 0;
  while (i < size1 && j < size2) {
    if (array1[i] > array2[j]) {
      destination[i + j] = array2[j];
      ++j;
      ++swaps_done;
    } else {
      destination[i + j] = array1[i];
      counter += swaps_done;
      ++i;
    }
  }
  if (i == size1) {
    while (j < size2) {
      destination[i + j] = array2[j];
      ++j;
    }
  } else {
    while (i < size1) {
      destination[i + j] = array1[i];
      ++i;
      counter += swaps_done;
    }
  }
}

template <class T>
void MergeSort(T* array, size_t size, int64_t& counter) {
  if (size == 2) {
    if (array[0] > array[1]) {
      std::swap(array[0], array[1]);
      ++counter;
    }
    return;
  }
  if (size <= 1) {
    return;
  }
  size_t LeftHalf = size / 2;
  size_t RightHalf = size - LeftHalf;
  MergeSort(array, LeftHalf, counter);
  MergeSort(array + LeftHalf, RightHalf, counter);
  std::vector<T> temp = std::vector<T>(size);
  Merge(array, LeftHalf, array + LeftHalf, RightHalf, temp.data(), counter);
  memcpy(array, temp.data(), size * sizeof(T));
}

template <class T>
int64_t MergeSort(std::vector<T>& array) {
  int64_t counter = 0;
  MergeSort(array.data(), array.size(), counter);
  return counter;
}

int main() {
  std::vector<int> vector;
  int value = 0;
  while (std::cin >> value) {
    vector.emplace_back(value);
  }
  std::cout << MergeSort(vector) << std::endl;
}
