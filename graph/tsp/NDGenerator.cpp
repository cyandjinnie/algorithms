//
// Created by Igor Maximov on 2019-04-27.
//

#include "NDGenerator.h"

std::vector<double>&& NormalDistributionGenerator::Generate(double mean,
                                                            double stddev,
                                                            size_t cnt) {
  std::random_device random_device;
  std::mt19937 generator(random_device());
  auto distribution = std::normal_distribution<double>(mean, stddev);
  result_ = std::vector<double>();
  for (int i = 0; i < cnt; ++i) {
    result_.push_back(distribution(generator));
  }
  return std::move(result_);
}
