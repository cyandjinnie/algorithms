//
// Created by Igor Maximov on 2019-04-27.
//

#include "CoordinateGraph.h"
#include <set>
#include <unordered_map>

template <typename WeightType>
WeightType CoordinateGraph<WeightType>::GetSquareWeight(Vertex from,
                                                        Vertex to) const {
  return std::pow(coord_x_[from] - coord_x_[to], 2) +
         std::pow(coord_y_[from] - coord_y_[to], 2);
}

template <typename WeightType>
WeightType CoordinateGraph<WeightType>::GetWeight(Vertex from,
                                                  Vertex to) const {
  return std::sqrt(std::pow(coord_x_[from] - coord_x_[to], 2) +
                   std::pow(coord_y_[from] - coord_y_[to], 2));
}

template <typename WeightType>
WeightType CoordinateGraph<WeightType>::FindMinimalSpanningTree() const {
  const WeightType inf = std::numeric_limits<WeightType>::max();
  WeightType weight_sum = 0;
  // (weight, from, to)
  std::set<std::pair<WeightType, Vertex>> edges;
  min_spanning_tree_ =
      std::vector<std::vector<Vertex>>(VerticesCount(), std::vector<Vertex>());
  std::unordered_map<Vertex, Vertex> parent;
  std::vector<WeightType> min_edge(VerticesCount(), inf);
  std::vector<bool> is_added(VerticesCount(), false);
  min_edge[0] = 0;
  is_added[0] = true;
  edges.emplace(0, 0);
  parent[0] = 0;
  for (int edges_added = 0; edges_added < VerticesCount(); ++edges_added) {
    auto [key, current] = *edges.begin();
    edges.erase(edges.begin());
    // min_spanning_tree_[current].push_back(parent[current]);
    min_spanning_tree_[parent[current]].push_back(current);
    weight_sum += key;
    is_added[current] = true;
    for (Vertex next = 0; next < VerticesCount(); ++next) {
      WeightType weight = GetWeight(current, next);
      if (weight < min_edge[next] && !is_added[next]) {
        parent[next] = current;
        edges.erase(std::make_pair(min_edge[next], next));
        min_edge[next] = weight;
        edges.emplace(min_edge[next], next);
      }
    }
  }
  return weight_sum;
}

template <typename WeightType>
WeightType CoordinateGraph<WeightType>::ApproximateTSP() const {
  FindMinimalSpanningTree();
  // Spanning tree traversal
  visited_.assign(min_spanning_tree_.size(), false);
  visited_[0] = true;
  MstDfs(0);
  WeightType length = GetWeight(0, path_[0]) +
                      GetWeight(path_[min_spanning_tree_.size() - 2], 0);
  for (int i = 1; i < min_spanning_tree_.size() - 1; ++i) {
    length += GetWeight(path_[i - 1], path_[i]);
  }
  return length;
}

template <typename WeightType>
void CoordinateGraph<WeightType>::MstDfs(Vertex from) const {
  for (Vertex next : min_spanning_tree_[from]) {
    if (!visited_[next]) {
      path_.push_back(next);
      visited_[next] = true;
      MstDfs(next);
    }
  }
}