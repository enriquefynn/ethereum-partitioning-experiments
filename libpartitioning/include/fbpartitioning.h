#pragma once

#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include <config.h>
#include <partitioner.h>

class FB_partitioner : public Partitioner {
  std::vector<uint32_t> get_neighbors(const std::vector<int32_t> &partitioning,
                                      const Graph &g);

public:
  FB_partitioner(const Graph &graph)
      : Partitioner(0, graph) {}
  void assign_partition(std::vector<int32_t> &partitioning,
                        uint32_t from_vertex, uint32_t to_vertex,
                        int32_t nparts);

  std::vector<int32_t> partition(int32_t nparts) {
    return std::vector<int32_t>();
  };

  bool trigger_partitioning(uint32_t new_timestamp,
                            bool last_edge_cross_partition) {
    return false;
  };
};