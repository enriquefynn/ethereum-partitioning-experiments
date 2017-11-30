#pragma once
#include <cassert>
#include <iostream>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include <config.h>
#include <partitioner.h>

class Hash_partitioner : public Partitioner {

public:
  Hash_partitioner(const Graph &graph) : Partitioner(0, graph) {}

void assign_partition(const std::set<uint32_t> &vertex_list, int32_t nparts);
  
  uint32_t partition(int32_t nparts) {
    return 0;
  };
  std::string get_name() { return "HASH"; }

  bool trigger_partitioning(uint32_t new_timestamp,
                            bool last_edge_cross_partition) {
    return false;
  };

  Hash_partitioner &operator=(const Hash_partitioner &) = delete;
  Hash_partitioner(const Hash_partitioner &) = delete;
};
