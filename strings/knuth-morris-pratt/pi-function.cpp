#include <iostream>
#include <string>
#include <vector>

std::vector<int> MatchPattern(const std::string& pattern, char divider,
                              const std::string& input) {
  std::string preprocessed_string = pattern + divider + input;
  // z-function values array
  std::vector<int> z_func(preprocessed_string.length(), 0);
  std::vector<int> matches;

  // left and right bounds of the right-most z-block
  int left = 0;
  int right = 0;
  // By definition
  z_func[0] = 0;
  for (int i = 1; i < preprocessed_string.length(); ++i) {
    z_func[i] = std::max(0, std::min(right - i, z_func[i - left]));
    while (i + z_func[i] < preprocessed_string.length() &&
           preprocessed_string[z_func[i]] ==
               preprocessed_string[i + z_func[i]]) {
      ++z_func[i];
    }
    if (i + z_func[i] > right) {
      left = i;
      right = i + z_func[i];
    }
    if (z_func[i] == pattern.length()) {
      matches.push_back(i - pattern.length() - 1);
    }
  }

  return matches;
}

int main() {
  std::string pattern;
  std::string input;
  char divider = '\n';
  std::cin >> pattern;
  std::cin >> input;
  std::vector<int> matches = MatchPattern(pattern, divider, input);
  for (auto pos : matches) {
    std::cout << pos << " ";
  }
  return 0;
}