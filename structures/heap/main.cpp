#include <algorithm>
#include <functional>
#include <iostream>
#include <stack>
#include <utility>
#include <vector>

template <typename T, typename Comparator>
class BinaryHeap {
 public:
  BinaryHeap(T* array, const int size, Comparator compare)
      : buffer_(array), size_(size), compare(compare) {
    for (int i = (size / 2); i >= 0; --i) {
      SiftDown(i);
    }
  }

  void Sort() {
    for (int i = size_ - 1; i > 0; --i) {
      buffer_[i] = this->Extract();
    }
  }

  T Extract() {
    T answer = buffer_[0];
    buffer_[0] = buffer_[size_ - 1];
    SiftDown(0);
    --size_;
    return answer;
  }

 private:
  int GetLeftChildId(const int index) const { return 2 * index + 1; }

  int GetRightChildId(const int index) const { return 2 * index + 2; }

  void SiftDown(const int index) {
    int left;
    int right;
    int i = index;
    int j = 0;
    while (GetLeftChildId(i) < size_) {
      left = GetLeftChildId(i);
      right = GetRightChildId(i);
      j = left;
      if (right < size_ && compare(buffer_[left], buffer_[right])) {
        j = right;
      }
      if (compare(buffer_[j], buffer_[i])) {
        break;
      }
      std::swap(buffer_[i], buffer_[j]);
      i = j;
    }
  }

 private:
  Comparator compare;
  T* buffer_;
  int size_;
};

template <typename T, typename Comparator = std::less<T>>
void HeapSort(std::vector<T>& vector, Comparator compare = Comparator()) {
  auto heap = BinaryHeap<T, Comparator>(vector.data(), vector.size(), compare);
  heap.Sort();
}

struct GuestLog {
  int entrance_time;
  int exit_time;
  int ads_shown;

  GuestLog() = default;

  GuestLog(int entrance_time, int exit_time)
      : entrance_time(entrance_time), exit_time(exit_time), ads_shown(0) {}

  static bool Compare(const GuestLog& lhs, const GuestLog& rhs) {
    return lhs.exit_time < rhs.exit_time
               ? true
               : lhs.exit_time == rhs.exit_time
                     ? lhs.entrance_time > rhs.entrance_time
                     : false;
  }

  static void ShowAd(std::vector<GuestLog>& guests, const int start_index,
                     const int advert_time) {
    for (int i = start_index; i < guests.size(); ++i) {
      if (guests[i].entrance_time <= advert_time) {
        ++guests[i].ads_shown;
      }
    }
  }
};

int main() {
  size_t customers_count = 0;
  std::cin >> customers_count;
  std::vector<GuestLog> guests;
  guests.reserve(customers_count);
  for (int i = 0; i < customers_count; ++i) {
    int entrance_time = 0;
    int exit_time = 0;
    std::cin >> entrance_time >> exit_time;
    guests.emplace_back(entrance_time, exit_time);
  }
  HeapSort(guests, GuestLog::Compare);
  int ads_counter = 0;
  for (int i = 0; i < guests.size(); ++i) {
    if (guests[i].ads_shown == 0) {
      GuestLog::ShowAd(guests, i, guests[i].exit_time - 1);
      ++ads_counter;
    }
    if (guests[i].ads_shown == 1) {
      GuestLog::ShowAd(guests, i, guests[i].exit_time);
      ++ads_counter;
    }
  }
  std::cout << ads_counter << std::endl;
  return 0;
}
