#pragma once

#include <vector>
#include <metis.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include <config.h>

class FBPartitioning {
public:
  std::vector<uint32_t> get_neighbors(const std::vector<idx_t> &partitioning,
                                      const Graph &g);
};