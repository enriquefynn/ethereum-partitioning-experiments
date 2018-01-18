#pragma once

#include <config.h>
#include <iostream>
#include <log.h>
#include <set>
#include <unordered_map>
#include <vector>

class Partitioner {
protected:
  const int32_t m_seed;
  Graph &m_graph;
  Config &m_config;

  uint32_t m_total_calls = 0;
  uint32_t m_last_partitioning_time; // For saving the next partitioning
  uint32_t m_timestamp_last_repartition = 0;
  uint32_t m_timestamp_last_check = 0;
  uint32_t m_cross_partition_calls;

public:
  std::map<uint32_t, Vertex> m_id_to_vertex;
  std::unordered_map<uint32_t, uint32_t> m_partitioning;
  std::vector<uint32_t> m_balance;

  Partitioner(int32_t seed, Graph &graph, Config &config)
      : m_seed(seed), m_graph(graph), m_config(config),
        m_last_partitioning_time(0), m_balance(m_config.N_PARTITIONS, 0){};

  virtual uint32_t partition(int32_t n_part) = 0;
  virtual bool trigger_partitioning(uint32_t new_timestamp,
                                    uint32_t cross_edge_access,
                                    uint32_t same_partition_edge_access);
  virtual std::string get_name() = 0;

  virtual void assign_partition(const std::set<uint32_t> &vertex_list,
                                int32_t nparts);

  template <typename map_type>
  void define_partitioning(map_type &&new_partitioning) {
    assert(new_partitioning.size() == boost::num_vertices(m_graph));
    m_partitioning = std::move(new_partitioning);
  }

  virtual void remove_vertex(uint32_t vtx);

  virtual const uint32_t calculate_movements_repartition(
      const std::unordered_map<uint32_t, uint32_t> &old_partitioning,
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
  virtual const std::tuple<uint32_t, std::vector<uint32_t>>
  calculate_edge_cut(const Graph &g);

  inline bool same_partition(uint32_t v1, uint32_t v2) const {
    try {
      return m_partitioning.at(v1) == m_partitioning.at(v2);
    } catch (const std::out_of_range &oor) {
      return true;
      assert(false);
    }
  }

  virtual void terminate() {}
};