#pragma once

#include <METIS_partitioning.h>
#include <config.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

class Future_partitioner : public METIS_partitioner {
private:
  std::ifstream m_partitioning_file;
  std::unordered_set<uint32_t> new_vertices;

public:
  Future_partitioner(Graph &graph, Config &config)
      : METIS_partitioner(graph, config) {
    assert(m_config.SAVE_PARTITIONING);
  }

  void assign_partition(const std::set<uint32_t> &vertex_list, int32_t nparts);

  uint32_t partition(int32_t nparts);
  std::string get_name() { return "Future"; };

  void terminate() {
    partition(m_config.N_PARTITIONS);
    m_config.FILE_INPUT.close();
  }

  void remove_vertex(uint32_t vtx) {}

  const uint32_t calculate_movements_repartition(
      const std::unordered_map<uint32_t, uint32_t> &old_partitioning,
      int32_t nparts) const {
    return 0;
  }
  // Future_partitioner &operator=(const Future_partitioner &) = delete;
  // Future_partitioner(const Future_partitioner &) = delete;
};
