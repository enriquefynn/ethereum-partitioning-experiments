#pragma once

#include <cassert>
#include <config.h>
#include <fstream>
#include <iostream>
#include <partitioner.h>
#include <vector>

class File_partitioner : public Partitioner {
private:
  std::ifstream m_partitioning_file;
  uint32_t m_partitioning_epoch;

public:
  File_partitioner(const Graph &graph, const Config &config);

  void assign_partition(const std::set<uint32_t> &vertex_list, int32_t nparts) {
    assert(*vertex_list.rbegin() < m_partitioning.size());
  }

  uint32_t partition(int32_t nparts);
  std::string get_name() { return "FILE"; };

  bool trigger_partitioning(uint32_t new_timestamp, uint32_t cross_edge_access,
                            uint32_t same_partition_edge_access);

  File_partitioner &operator=(const File_partitioner &) = delete;
  File_partitioner(const File_partitioner &) = delete;
};
