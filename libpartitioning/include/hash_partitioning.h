#pragma once
#include <cassert>
#include <iostream>
#include <vector>

#include <config.h>
#include <partitioner.h>

class Hash_partitioner : public Partitioner {

public:
  Hash_partitioner(Graph &graph, Config &config)
      : Partitioner(0, graph, config) {}

  void assign_partition(const std::set<uint32_t> &vertex_list, int32_t nparts);

  uint32_t partition(int32_t nparts) { return 0; };
  std::string get_name() { return "HASH"; }

  bool trigger_partitioning(uint32_t new_timestamp, uint32_t cross_edge_access,
                            uint32_t same_partition_edge_access,
                            const std::vector<uint32_t> &tx_per_partition) {
    return false;
  }

  Hash_partitioner &operator=(const Hash_partitioner &) = delete;
  Hash_partitioner(const Hash_partitioner &) = delete;
};
