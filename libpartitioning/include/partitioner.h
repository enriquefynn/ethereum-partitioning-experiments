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

  double
  calculate_partition_load(const std::vector<uint32_t> &txs_per_partition);

public:
  std::map<uint32_t, Vertex> m_id_to_vertex;
  std::unordered_map<uint32_t, uint32_t> m_partitioning;
  std::vector<uint32_t> m_balances;
  double m_balance;

  Partitioner(int32_t seed, Graph &graph, Config &config)
      : m_seed(seed), m_graph(graph), m_config(config),
        m_last_partitioning_time(0), m_balances(m_config.N_PARTITIONS, 0),
        m_balance(0){};

  virtual uint32_t partition(int32_t n_part) = 0;
  virtual bool
  trigger_partitioning(uint32_t new_timestamp, uint32_t cross_edge_access,
                       uint32_t same_partition_edge_access,
                       const std::vector<uint32_t> &tx_per_partition);

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
      const std::unordered_map<uint32_t, uint32_t> &new_partitioning,
      int32_t nparts) const;

  // Edges cut, vertices in each partitioning partitioning.size()
  virtual const std::tuple<uint32_t, std::vector<uint32_t>>
  calculate_edge_cut_balances(const Graph &g);

  bool same_partition(uint32_t v1, uint32_t v2) const;

  virtual void added_edge(uint32_t from, uint32_t to) {}
  virtual void terminate() {}
  virtual ~Partitioner() {}
};