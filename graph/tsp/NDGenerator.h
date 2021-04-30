//
// Created by Igor Maximov on 2019-04-27.
//

#ifndef INC_3_2_1_LIB_H
#define INC_3_2_1_LIB_H

#include <cmath>
#include <random>
#include <vector>

class NormalDistributionGenerator {
 public:
  NormalDistributionGenerator() = default;

  std::vector<double>&& Generate(double mean, double stddev, size_t cnt);

 private:
  std::vector<double> result_;
};

#endif  // INC_3_2_1_LIB_H
