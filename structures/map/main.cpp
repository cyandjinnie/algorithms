#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

/*
  Реализуйте структуру данных типа “множество строк” на основе динамической
  хеш-таблицы с открытой адресацией. Хранимые строки непустые и состоят из
  строчных латинских букв. Хеш-функция строки должна быть реализована с помощью
  вычисления значения многочлена методом Горнера. Начальный размер таблицы
  должен быть равным 8-ми. Перехеширование выполняйте при добавлении элементов
  в случае, когда коэффициент заполнения таблицы достигает 3/4. Структура данных
  должна поддерживать операции добавления строки в множество, удаления строки
  из множества и проверки принадлежности данной строки множеству. 1_1. Для
  разрешения коллизий используйте линейное пробирование. 1_2. Для разрешения
  коллизий используйте двойное хеширование.

*/

using std::shared_ptr;

template <typename T>
class Set {
 public:
  virtual bool Find(const T& key) const = 0;
  virtual bool Insert(const T& key) = 0;
  virtual bool Delete(const T& key) = 0;
};

class HashTable : Set<std::string> {
 public:
  HashTable() : capacity_(default_capacity_), buffer_(default_capacity_) {}

  bool Find(const std::string& key) const {
    size_t hash = Hash1(key);
    size_t hash2 = Hash2(key);
    int i = 0;
    while (i < capacity_) {
      if (buffer_[hash] == nullptr) {
        return false;
      } else if (buffer_[hash]->string == key) {
        return !buffer_[hash]->IsDeleted;
      }
      hash = (hash + hash2) % capacity_;
      ++i;
    }
    return false;
  }

  bool Insert(const std::string& key) {
    if (Find(key)) {
      return false;
    }
    size_t hash = Hash1(key);
    size_t hash2 = Hash2(key);
    int i = 0;
    while (i < capacity_) {
      if (buffer_[hash] == nullptr) {
        Allocate(buffer_[hash], key);
        size_++;
        if (CoefficientOnInsertion() >= 0.75) {
          Rehash();
        }
        return true;
      } else if (buffer_[hash]->IsDeleted) {
        Reset(buffer_[hash], key);
        size_++;
        if (CoefficientOnInsertion() >= 0.75) {
          Rehash();
        }
        return true;
      }
      ++i;
      hash = (hash + hash2) % capacity_;
    }
    assert(false);
  }

  bool Delete(const std::string& key) {
    if (!Find(key)) {
      return false;
    }
    size_t hash = Hash1(key);
    size_t hash2 = Hash2(key);
    int i = 0;
    while (i < capacity_) {
      if (buffer_[hash] == nullptr) {
        return false;
      } else if (buffer_[hash]->string == key) {
        if (!buffer_[hash]->IsDeleted) {
          buffer_[hash]->IsDeleted = true;
          --size_;
          return true;
        } else {
          return false;
        }
      }
      ++i;
      hash = (hash + hash2) % capacity_;
    }
    assert(false);
  }

 private:
  struct TableEntry {
    TableEntry(const std::string& string_init) : string(string_init) {}
    std::string string;
    bool IsDeleted = false;
  };

  using EntryPtr = shared_ptr<TableEntry>;

  float CoefficientOnInsertion() {
    return static_cast<float>(size_ / capacity_);
  }

  void Allocate(EntryPtr& ptr, const std::string& key) {
    ptr = std::make_shared<TableEntry>(key);
  }

  void Reset(EntryPtr& ptr, const std::string& key) {
    ptr->string = key;
    ptr->IsDeleted = false;
  }

  size_t Hash1(std::string string) const {
    size_t result = 0;
    for (int i = 0; i < string.size(); ++i) {
      result = ((result + static_cast<size_t>(string[i])) * prime1) % capacity_;
    }
    return result;
  }

  size_t Hash2(std::string string) const {
    size_t result = 0;
    for (int i = 0; i < string.size(); ++i) {
      result = ((result + static_cast<size_t>(string[i])) * prime2) % capacity_;
    }
    return 2 * result + 1;
  }

  void Rehash() {
    std::vector<EntryPtr> temp(buffer_);
    capacity_ *= 2;
    buffer_.assign(capacity_, nullptr);
    size_ = 0;
    for (int i = 0; i < capacity_ / 2; ++i) {
      if (temp[i] != nullptr) {
        Insert(temp[i]->string);
      }
    }
  }

 private:
  const size_t prime1 = 59359;
  const size_t prime2 = 31;
  const size_t default_capacity_ = 8;
  size_t capacity_;
  size_t size_ = 0;
  std::vector<EntryPtr> buffer_;
};

int main() {
  enum CommandType : char {
    Undefined = 0,
    Insert = '+',
    Find = '?',
    Delete = '-'
  };
  char command = CommandType::Undefined;
  std::string string = std::string();
  auto table = HashTable();
  while (std::cin >> command >> string) {
    bool ok = false;
    switch (command) {
      case Insert:
        ok = table.Insert(string);
        break;
      case Delete:
        ok = table.Delete(string);
        break;
      case Find:
        ok = table.Find(string);
        break;
      default:
        assert(false);
        break;
    }
    if (ok) {
      std::cout << "OK" << std::endl;
    } else {
      std::cout << "FAIL" << std::endl;
    }
  }
}
