//
// Created by Igor Maximov on 2019-04-27.
//

#ifndef INC_3_2_1_FULLYCONNECTEDGRAPH_H
#define INC_3_2_1_FULLYCONNECTEDGRAPH_H

#include <cmath>
#include <iostream>
#include <vector>
#include "Constant.h"

template <typename WeightType>
class CoordinateGraph {
 public:
  CoordinateGraph(const std::vector<WeightType>& coord_x,
                  const std::vector<WeightType>& coord_y)
      : coord_x_(coord_x), coord_y_(coord_y) {}

  CoordinateGraph(std::vector<WeightType>&& coord_x,
                  std::vector<WeightType>&& coord_y)
      : coord_x_(coord_x), coord_y_(coord_y) {}

  typedef WeightType Type;

  void Print() const {
    std::cout << "Points: " << std::endl;
    for (Vertex j = 0; j < VerticesCount(); ++j) {
      std::cout << "(" << coord_x_[j] << ", " << coord_y_[j] << ")"
                << " ";
    }
    std::cout << "Matrix: " << std::endl;
    for (Vertex i = 0; i < VerticesCount(); ++i) {
      for (Vertex j = 0; j < VerticesCount(); ++j) {
        std::cout << GetWeight(i, j) << " ";
      }
      std::cout << std::endl;
    }
  }

  WeightType FindMinimalSpanningTree() const;

  WeightType ApproximateTSP() const;

  WeightType GetWeight(Vertex from, Vertex to) const;

  WeightType GetSquareWeight(Vertex from, Vertex to) const;

  size_t VerticesCount() const { return coord_x_.size(); }

 private:
  void MstDfs(Vertex from) const;

  mutable std::vector<Vertex> path_;
  mutable std::vector<bool> visited_;
  std::vector<WeightType> coord_x_;
  std::vector<WeightType> coord_y_;
  mutable std::vector<std::vector<Vertex>> min_spanning_tree_;
};

#endif  // INC_3_2_1_FULLYCONNECTEDGRAPH_H
