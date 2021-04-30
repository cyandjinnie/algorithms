//
// Created by Igor Maximov on 2019-04-29.
//

#include "Tests.h"
#include <iostream>
#include "CoordinateGraph.cpp"
#include "CoordinateGraph.h"
#include "NDGenerator.h"
#include "TSPSolver.h"

using std::cin;
using std::cout;
using std::endl;

void ApproximationTest(int num_iterations, int vertices_left_bound,
                       int vertices_right_bound, bool print_logs) {
  const double random_mean = 0;
  const double stddev = 10;
  const int num_points = 10;
  const int two = 2;
  for (int i = vertices_left_bound; i <= vertices_right_bound; ++i) {
    double mean_squared_error = 0;
    double mean;

    for (int j = 0; j < num_iterations; ++j) {
      auto gen1 = NormalDistributionGenerator();
      auto gen2 = NormalDistributionGenerator();

      auto graph =
          CoordinateGraph(gen1.Generate(random_mean, stddev, num_points),
                          gen2.Generate(random_mean, stddev, num_points));

      double approximate = graph.ApproximateTSP();
      double precise = TSPSolver<double>(graph)();
      double ratio = approximate / precise;

      if (print_logs) {
        cout << "ratio: " << ratio << endl;
      }

      mean += ratio;
      mean_squared_error += std::pow(ratio - 1, two);

      // std::cout << approximate << std::endl;
      // std::cout << precise << std::endl;
    }
    mean /= num_iterations;
    mean_squared_error /= num_iterations;
    cout << "N = " << i << ": " << endl;
    cout << "Arithmetic mean = " << mean << endl;
    cout << "MSE = " << mean_squared_error << endl;
  }
}