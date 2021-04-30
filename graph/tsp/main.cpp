#include <cassert>
#include <iostream>
#include "Tests.h"

//#define DEBUG

using std::cin;
using std::cout;
using std::endl;

int main() {
  int num_iterations = 0;
  int vertices_left_bound = 0;
  int vertices_right_bound = 0;
  std::string input;
  bool print_logs = false;

  cout << "Running test for number of vertices ";
  cout << "in range [a, b]" << endl;
  cout << "a = ";
  cin >> vertices_left_bound;
  cout << "b = ";
  cin >> vertices_right_bound;
  assert(vertices_left_bound <= vertices_right_bound);

  cout << "Number of iterations: ";
  cin >> num_iterations;

  cout << "Print logs? [y/n]: ";
  cin >> input;
  if (input == "y") {
    print_logs = true;
  }

  ApproximationTest(num_iterations, vertices_left_bound, vertices_right_bound,
                    print_logs);
}