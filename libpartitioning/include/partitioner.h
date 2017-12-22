#pragma once

#include <config.h>
#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>
#include <log.h>

class Partitioner {
protected:
  const int32_t m_seed;
  const Graph &m_graph;
  Config &m_config;
  uint32_t m_last_partitioning_time; // For saving the next partitioning

public:
  std::unordered_map<uint32_t, uint32_t> m_partitioning;
  std::vector<uint32_t> m_balance;
  Partitioner(int32_t seed, const Graph &graph, Config &config)
      : m_seed(seed), m_graph(graph), m_config(config),
        m_last_partitioning_time(0), m_balance(m_config.N_PARTITIONS, 0){};

  virtual uint32_t partition(int32_t n_part) = 0;
  virtual bool trigger_partitioning(uint32_t new_timestamp,
                                    uint32_t cross_edge_access,
                                    uint32_t same_partition_edge_access) = 0;
  virtual std::string get_name() = 0;

  virtual void assign_partition(const std::set<uint32_t> &vertex_list,
                                int32_t nparts);

  template <typename map_type>
  void define_partitioning(map_type &&new_partitioning) {
    assert(new_partitioning.size() == boost::num_vertices(m_graph));
    m_partitioning = std::move(new_partitioning);
  }

  virtual void remove_vertex(uint32_t vtx) {
    // std::cout << "SIZE: " << m_partitioning.size() << std::endl;
    // assert(vtx < m_partitioning.size());
    assert(m_partitioning[vtx] < m_config.N_PARTITIONS);
    assert(m_balance[m_partitioning[vtx]] > 0);

    --m_balance[m_partitioning[vtx]];
    m_partitioning.erase(vtx);
    m_config.m_id_to_vertex.erase(vtx);
  }

  template <typename map_type>
  const uint32_t calculate_movements_repartition(map_type &old_partitioning,
                                                 int32_t nparts) const {
    uint32_t moves = 0;
    assert(m_partitioning.size() == old_partitioning.size());

    for (const auto &kv : old_partitioning) {
      if (kv.second != m_partitioning.at(kv.first))
        ++moves;
    }
    return moves;
  }

  // Edges cut, vertices in each partitioning partitioning.size()
  const std::tuple<uint32_t, std::vector<uint32_t>>
  calculate_edge_cut(const Graph &g);

  inline bool same_partition(uint32_t v1, uint32_t v2) const {
    try {
      return m_partitioning.at(v1) == m_partitioning.at(v2);
    } catch (const std::out_of_range &oor) {
      return true;
      assert(false);
    }
  }
};