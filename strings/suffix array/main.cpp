#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <unordered_map>

struct PairHash {
    public:
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &x) const
    {
        return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
    }
};

const int ascii_table_size = 128;

//#define DEBUG

void PrintPairArray(const std::vector<std::pair<int, int>>& array) {
    for (auto&& [first, second] : array) {
        std::cout << "(" << first << ", " << second << ")\n";
    }
}

std::vector<std::pair<int, int>> SortPairArrayLSD(const std::vector<std::pair<int, int>>& array, int domain_size) {
    std::vector<int> cnt(domain_size, 0);
    std::vector<int> accumulated_offset(domain_size, 0);

    for (int i = 0; i < domain_size; ++i) {
        ++cnt[array[i].second];
    }

    int current_offset = 0;
    for (int i = 0; i < cnt.size(); ++i) {
        if (cnt[i] != 0) {
            accumulated_offset[i] = current_offset;
            current_offset += cnt[i];
        }
    }

    std::vector<std::pair<int, int>> sorted_by_second(domain_size);
    for (int i = 0; i < array.size(); ++i) {
        int offset = accumulated_offset[array[i].second];
        sorted_by_second[offset] = array[i];
        accumulated_offset[array[i].second] += 1;
    }

#ifdef DEBUG
    std::cout << "-------- <Sorted by second index> ---------\n";
  PrintPairArray(sorted_by_second);
#endif

    cnt.assign(domain_size, 0);
    accumulated_offset.assign(domain_size, 0);

    for (int i = 0; i < sorted_by_second.size(); ++i) {
        ++cnt[sorted_by_second[i].first];
    }

    current_offset = 0;
    for (int i = 0; i < cnt.size(); ++i) {
        if (cnt[i] != 0) {
            accumulated_offset[i] = current_offset;
            current_offset += cnt[i];
        }
    }

    std::vector<std::pair<int, int>> sorted_by_first(sorted_by_second.size());
    for (int i = 0; i < sorted_by_second.size(); ++i) {
        int offset = accumulated_offset[sorted_by_second[i].first];
        sorted_by_first[offset] = sorted_by_second[i];
        accumulated_offset[sorted_by_second[i].first] += 1;
    }

#ifdef DEBUG
    std::cout << "-------- <Sorted by first index> ---------\n";
  PrintPairArray(sorted_by_first);
#endif

    return sorted_by_first;
}

void SuffixArrayCountSortPhase(const std::string& string, std::vector<int>& substring_order, std::vector<int>& classes) {
    std::vector<int> char_cnt(ascii_table_size, 0);

    for (int i = 0; i < string.length(); ++i) {
        char_cnt[string[i]] += 1;
    }

    std::vector<int> accumulated_offset(ascii_table_size, 0);
    std::vector<int> char_order(ascii_table_size, 0);
    int current_offset = 0;
    int num_chars_before = 0;

    for (int i = 0; i < char_cnt.size(); ++i) {
        if (char_cnt[i] != 0) {
            accumulated_offset[i] = current_offset;
            current_offset += char_cnt[i];

            char_order[i] = num_chars_before;
            ++num_chars_before;
        }
    }

    // char_cnt now stores number of previously read symbols
    char_cnt.assign(ascii_table_size, 0);

    for (int i = 0; i < string.length(); ++i) {
        classes[i] = char_order[string[i]];
        substring_order[i] = accumulated_offset[string[i]] + char_cnt[string[i]];
        char_cnt[string[i]] += 1;
    }
}

void SuffixArrayLSDSort(std::vector<int>& classes, std::vector<int>& substring_order, int num_iteration) {
    std::vector<std::pair<int, int>> substring_indices;
    int string_length = std::exp2(num_iteration);
    int n = classes.size();
    int current_len = std::exp2(num_iteration);

    // Building array of pairs (c1, c2) corresponding to a substring
    // of length 2^num_iteration
    // [c1, c2 - equivalence classes of substrings of len 2^(num_iteration - 1)]
    for (int i = 0; i < n; ++i) {
        int second_index = (i + current_len) % n;
        substring_indices.emplace_back(std::make_pair(classes[i], classes[second_index]));
    }
    auto sorted = SortPairArrayLSD(substring_indices, n);

    std::unordered_map<std::pair<int, int>, int, PairHash> pair_count;
    for (auto&& pair : sorted) {
        pair_count[pair] = 0;
    }
    for (auto&& pair : sorted) {
        pair_count[pair] += 1;
    }

    std::unordered_map<std::pair<int, int>, int, PairHash> accumulated_offset;
    std::unordered_map<std::pair<int, int>, int, PairHash> equivalence_class_ord;
    std::pair<int, int> previous_pair = std::make_pair(-1, -1);
    int current_offset = 0;
    int num_classes_before = 0;
    for (auto&& current_pair : sorted) {
        if (previous_pair != current_pair) {
            accumulated_offset[current_pair] = current_offset;
            current_offset += pair_count[current_pair];

            equivalence_class_ord[current_pair] = num_classes_before;
            ++num_classes_before;
        }
    }

    for (int i = 0; i < substring_indices.size(); ++i) {
        classes[i] = equivalence_class_ord[substring_indices[i]];
        substring_order[i] = accumulated_offset[substring_indices[i]];
        accumulated_offset[substring_indices[i]] += 1;
    }
}

std::vector<int> BuildSuffixArray(const std::string& input) {
    std::string string = input;
    string += '$';

    std::vector<int> classes(string.size(), 0);
    std::vector<int> substring_order(string.size(), 0);
    SuffixArrayCountSortPhase(string, substring_order, classes);

#ifdef DEBUG
    std::cout << "ch\tclasses\tsubstring_order\n";
  for (int i = 0; i < string.length(); ++i) {
    std::cout << string[i] << "\t" << classes[i] << "\t" << substring_order[i] << "\n";
  }
  std::cout << "------- <LSD sort phase> -------\n";
#endif // DEBUG

    int n = std::ceil(std::log2(string.length()));
    for (int i = 0; i < n; ++i) {
        SuffixArrayLSDSort(classes, substring_order, i);
#ifdef DEBUG
        std::cout << "ch\tclasses\tsubstring_order\n";
    for (int i = 0; i < string.length(); ++i) {
      std::cout << string[i] << "\t" << classes[i] << "\t" << substring_order[i] << "\n";
    }
#endif
    }

    std::vector<int> suff_array(string.size(), 0);
    for (int i = 0; i < substring_order.size(); ++i) {
        suff_array[substring_order[i]] = i;
    }

    return suff_array;
}

std::vector<int> BuildSuffixesLCP(const std::string& input, const std::vector<int>& suff_array) {
    std::string string = input + '$';
    int str_length = string.length();
    std::vector<int> lcp(str_length, 0);
    std::vector<int> inverse_suff_array(str_length, 0);

    for (int i = 0; i < str_length; ++i) {
        inverse_suff_array[suff_array[i]] = i;
    }

    int k = 0;
    for (int i = 0; i < str_length; ++i) {
        if (k > 0) {
            --k;
        }
        if (inverse_suff_array[i] == str_length - 1) {
            lcp[str_length - 1] = -1;
            k = 0;
        } else {
            int j = suff_array[inverse_suff_array[i] + 1];
            while (std::max(i + k, j + k) < str_length && input[i + k] == input[j + k]) {
                ++k;
            }
            lcp[inverse_suff_array[i]] = k;
        }
    }

    return lcp;
}

int NumberDistinctSubstrings(const std::string& input) {
    std::vector<int> suff_array = BuildSuffixArray(input);

#ifdef DEBUG
    for (auto elem : suff_array) {
    std::cout << elem << ", ";
  }
  std::cout << "\n";
#endif // DEBUG

    std::vector<int> lcp = BuildSuffixesLCP(input, suff_array);

#ifdef DEBUG
    for (auto elem : lcp) {
    std::cout << elem << ", ";
  }
  std::cout << "\n";
#endif // DEBUG

    int result = 0;
    for (int i = 0; i < input.length(); ++i) {
        result += (i + 1) - lcp[i];
    }

    return result;
}

int main() {
    std::string string;
    std::cin >> string;

    std::cout << NumberDistinctSubstrings(string);

    return 0;
}
