#pragma once

#include <config.h>
#include <partitioner.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <vector>

class File_partitioner : public Partitioner {
private:
  uint32_t m_partitioning_epoch;
  std::unordered_set<uint32_t> new_vertices;

public:
  File_partitioner(Graph &graph, Config &config);

  void assign_partition(const std::set<uint32_t> &vertex_list, int32_t nparts) {
    for (auto const vtx : vertex_list) {
      if (!m_partitioning.count(vtx)) {
        LOG_INFO("%d missing", vtx);
        throw std::logic_error("Last partitioning reached in File partitioner");
      }
      if (!new_vertices.count(vtx)) {
        ++m_balances[m_partitioning[vtx]];
        new_vertices.insert(vtx);
      }
    }
  }

  void remove_vertex(uint32_t vtx) {
    assert(m_partitioning[vtx] < m_config.N_PARTITIONS);
    assert(m_balances[m_partitioning[vtx]] > 0);

    auto v_fr = m_id_to_vertex.find(vtx);
    if (v_fr == m_id_to_vertex.end())
      return;
    auto delete_vertex = (*v_fr).second;
    // std::cout << "REMOVE: " << delete_vertex << std::endl;
    boost::clear_vertex(delete_vertex, m_graph);
    boost::remove_vertex(delete_vertex, m_graph);

    --m_balances[m_partitioning[vtx]];
    m_id_to_vertex.erase(vtx);
  }

  uint32_t partition(int32_t nparts);
  std::string get_name() { return "FILE"; };

  bool trigger_partitioning(uint32_t new_timestamp, uint32_t cross_edge_access,
                            uint32_t same_partition_edge_access,
                            const std::vector<uint32_t> &tx_per_partition);

  File_partitioner &operator=(const File_partitioner &) = delete;
  File_partitioner(const File_partitioner &) = delete;

  const std::tuple<uint32_t, std::vector<uint32_t>>
  calculate_edge_cut_balances(const Graph &g);
};
