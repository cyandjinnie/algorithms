#include <string.h>
#include <cassert>
#include <cstdint>
#include <iostream>

/*
    Реализовать дек с динамическим зацикленным буфером.
    Обрабатывать команды push * и pop *.
*/

namespace MyLib {
class Deque {
 public:
  Deque() : buffer_(nullptr), head_(0), tail_(0), capacity_(0) {}

  explicit Deque(const size_t size)
      : buffer_(new int[size]),
        head_(size / 2),
        tail_(size / 2),
        capacity_(size) {
    assert(size > 0);
    memset(buffer_, 0, sizeof(int) * capacity_);
  }

  ~Deque() { delete[] buffer_; }

  void PushFront(int value) {
    if (IsFull()) {
      ReallocateMemory();
    }
    head_ = (head_ + 1) % capacity_;
    buffer_[head_] = value;
  }

  void PushBack(int value) {
    if (IsFull()) {
      ReallocateMemory();
    }
    buffer_[tail_] = value;
    tail_ = (tail_ - 1 + capacity_) % capacity_;
  }

  int PopBack() {
    if (IsEmpty()) {
      return -1;
    }
    tail_ = (tail_ + 1) % capacity_;
    int answer = buffer_[tail_];
    buffer_[tail_] = 0;
    return answer;
  }

  int PopFront() {
    if (IsEmpty()) {
      return -1;
    }
    int answer = buffer_[head_];
    buffer_[head_] = 0;
    head_ = (head_ - 1 + capacity_) % capacity_;
    return answer;
  }

  bool IsEmpty() const { return head_ == tail_; }

  bool IsFull() const { return GetSize() + 1 == capacity_; }

  size_t GetSize() const { return (head_ - tail_ + capacity_) % capacity_; }

 private:
  void ReallocateMemory() {
    int* temp_buffer = new int[capacity_ * 3];
    memset(temp_buffer, 0, sizeof(int) * capacity_ * 3);
    size_t new_tail = capacity_ + 1;
    for (int i = 0; i < capacity_; ++i) {
      temp_buffer[new_tail + i] = buffer_[(i + tail_) % capacity_];
    }
    temp_buffer[new_tail] = 0;
    tail_ = new_tail;
    head_ = tail_ + capacity_ - 1;
    delete[] buffer_;
    buffer_ = temp_buffer;
    capacity_ = capacity_ * 3;
  }

 private:
  int* buffer_;
  size_t head_;
  size_t tail_;
  size_t capacity_;
};
}  // namespace MyLib

int main() {
  enum OperationType {
    Undefined = 0,
    PushFront = 1,
    PopFront = 2,
    PushBack = 3,
    PopBack = 4
  };
  bool passed_all_tests = true;
  int operation_count = 0;
  std::cin >> operation_count;
  MyLib::Deque deq = MyLib::Deque(operation_count);
  for (int i = 0; i < operation_count; ++i) {
    int command = 0;
    int value = 0;
    std::cin >> command >> value;
    switch (command) {
      case PushFront: {
        deq.PushFront(value);
        break;
      }
      case PopFront: {
        if (value != deq.PopFront()) {
          passed_all_tests = false;
        }
        break;
      }
      case PushBack: {
        deq.PushBack(value);
        break;
      }
      case PopBack: {
        if (value != deq.PopBack()) {
          passed_all_tests = false;
        }
        break;
      }
    }
  }
  if (passed_all_tests) {
    std::cout << "YES" << std::endl;
  } else {
    std::cout << "NO" << std::endl;
  }
  return 0;
}
