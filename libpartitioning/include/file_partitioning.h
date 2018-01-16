#pragma once

#include <config.h>
#include <partitioner.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

class File_partitioner : public Partitioner {
private:
  std::ifstream m_partitioning_file;
  uint32_t m_partitioning_epoch;
  std::unordered_set<uint32_t> new_vertices;

public:
  File_partitioner(Graph &graph, Config &config);

  void assign_partition(const std::set<uint32_t> &vertex_list, int32_t nparts) {
    for (auto const vtx : vertex_list) {
      if (!m_partitioning.count(vtx))
        throw std::logic_error("Last partitioning reached in File partitioner");
      if (!new_vertices.count(vtx)) {
        ++m_balance[m_partitioning[vtx]];
        new_vertices.insert(vtx);
      }
    }
  }

  void remove_vertex(uint32_t vtx) {
    // std::cout << "SIZE: " << m_partitioning.size() << std::endl;
    // assert(vtx < m_partitioning.size());
    assert(m_balance[m_partitioning[vtx]] > 0);

    --m_balance[m_partitioning[vtx]];
    m_id_to_vertex.erase(vtx);
  }

  uint32_t partition(int32_t nparts);
  std::string get_name() { return "FILE"; };

  bool trigger_partitioning(uint32_t new_timestamp, uint32_t cross_edge_access,
                            uint32_t same_partition_edge_access);

  File_partitioner &operator=(const File_partitioner &) = delete;
  File_partitioner(const File_partitioner &) = delete;

  const std::tuple<uint32_t, std::vector<uint32_t>>
  calculate_edge_cut(const Graph &g);
};
