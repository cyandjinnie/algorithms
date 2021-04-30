//
// Created by Igor Maximov on 2019-04-27.
//

#ifndef INC_3_2_1_TSPSOLVER_H
#define INC_3_2_1_TSPSOLVER_H

#include "CoordinateGraph.h"

template <typename WeightType>
class TSPSolver {
 public:
  explicit TSPSolver(const CoordinateGraph<WeightType>& graph)
      : graph_(graph), dp_() {}

  const WeightType inf = std::numeric_limits<WeightType>::max();

  WeightType operator()() {
    dp_ = std::vector<std::vector<WeightType>>(
        graph_.VerticesCount(),
        std::vector<WeightType>(std::pow(2, graph_.VerticesCount()), inf));
    // for (Vertex i = 0; i < graph_.VerticesCount(); ++i) {
    //  for (uint64_t mask = 0; mask < std::pow(2, graph_.VerticesCount());
    //  ++mask) {
    //    dp_[i][mask] = inf;
    //  }
    //}
    dp_[0][0] = 0;
    return MinHamiltonianCycleRecursive(
        0, std::pow(2, graph_.VerticesCount()) - 1);
  }

  WeightType MinHamiltonianCycleRecursive(Vertex i, uint64_t mask) {
    if (dp_[i][mask] != inf) {
      auto ans = dp_[i][mask];
      return ans;
    }
    for (Vertex j = 0; j < graph_.VerticesCount(); ++j) {
      if (GetBitByPos(mask, j)) {
        dp_[i][mask] = std::min(dp_[i][mask], MinHamiltonianCycleRecursive(
                                                  j, mask - std::pow(2, j)) +
                                                  graph_.GetWeight(i, j));
      }
    }
    auto ans = dp_[i][mask];
    return ans;
  }

  static bool GetBitByPos(uint64_t bitset, int pos) {
    return (bitset & (1ULL << pos)) > 0;
  }

 private:
  std::vector<std::vector<WeightType>> dp_;
  const CoordinateGraph<WeightType>& graph_;
};

#endif  // INC_3_2_1_TSPSOLVER_H
