#include <string.h>
#include <cassert>
#include <iostream>

/*
    В круг выстроено N человек, пронумерованных числами от 1 до N.
    Будем исключать каждого k-ого до тех пор, пока не уцелеет
    только один человек.
    Необходимо определить номер уцелевшего.
    N, k ≤ 10000.
*/

namespace MyLib {
template <class T>
class Vector {
 public:
  explicit Vector(const int size)
      : buffer_(new T[size]), size_(size), capacity_(size) {}

  ~Vector() { delete[] buffer_; }

  T& operator[](const int index) const {
    assert(index >= 0);
    return buffer_[index];
  }

  void Erase(const int index) {
    assert(index >= 0);
    assert(index < size_);
    memcpy(buffer_ + index, buffer_ + index + 1,
           sizeof(T) * (size_ - index - 1));
    --size_;
  }

  const int GetSize() const { return size_; }

 private:
  T* buffer_ = nullptr;
  int size_;
  int capacity_;
};
}  // namespace MyLib

inline const int CalculateNextExclusion(const int current_number,
                                        const int number_of_people,
                                        const int exclude_each, const int i) {
  return (current_number + (exclude_each - 1)) % (number_of_people - i);
}

int GetLastMan(const int number_of_people, const int exclude_each) {
  assert(number_of_people > 0);
  assert(exclude_each > 0);
  MyLib::Vector<int> array(number_of_people);
  for (int i = 0; i < array.GetSize(); ++i) {
    array[i] = i + 1;
  }
  int next_exclusion = 0;
  for (int i = 0; i < number_of_people; ++i) {
    next_exclusion = CalculateNextExclusion(next_exclusion, number_of_people,
                                            exclude_each, i);
    array.Erase(next_exclusion);
  }
  return array[0];
}

int main() {
  int number_of_people = 0;
  int exclude_each = 0;
  std::cin >> number_of_people >> exclude_each;
  std::cout << GetLastMan(number_of_people, exclude_each) << std::endl;
  return 0;
}
