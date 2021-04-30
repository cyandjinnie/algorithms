#include <cassert>
#include <iostream>

/*
    Для сложения чисел используется старый компьютер.
    Время, затрачиваемое на нахождение суммы двух чисел равно их сумме.
    Требуется написать программу, которая определяет минимальное время,
    достаточное для вычисления суммы заданного набора чисел.
*/

void Swap(int& lhs, int& rhs) {
  int temp = lhs;
  lhs = rhs;
  rhs = temp;
}

void BubbleSort(int* array, const int size) {
  assert(size > 0);
  bool is_altered = false;
  do {
    is_altered = false;
    for (int i = 0; i < size - 1; ++i) {
      if (array[i] > array[i + 1]) {
        Swap(array[i], array[i + 1]);
        is_altered = true;
      }
    }
  } while (is_altered);
}

int FindTime(int* array, const int size) {
  assert(size > 0);
  BubbleSort(array, size);
  int time = 0;
  int current_sum = 0;
  for (int i = 0; i < size - 1; ++i) {
    current_sum = array[i] + array[i + 1];
    time += current_sum;
    // Adding new sum as an element to the array
    // and moving it until array is sorted
    array[i + 1] = current_sum;
    for (int j = i + 1; j < size - 1; ++j) {
      if (array[j] > array[j + 1]) {
        Swap(array[j], array[j + 1]);
      }
    }
  }
  return time;
}

int main() {
  int values_count;
  std::cin >> values_count;
  int* array = new int[values_count];
  for (int i = 0; i < values_count; i++) {
    std::cin >> array[i];
  }
  std::cout << FindTime(array, values_count) << std::endl;
  delete[] array;
  return 0;
}
