#pragma once

#include <config.h>
#include <set>
#include <vector>

class Partitioner {
protected:
  const int32_t m_seed;
  const Graph &m_graph;

public:
  std::vector<uint32_t> m_balance;
  Partitioner(int32_t seed, const Graph &graph)
      : m_seed(seed), m_graph(graph), m_balance(N_PARTITIONS, 0){};

  virtual std::vector<int32_t> partition(int32_t n_part) = 0;
  virtual bool trigger_partitioning(uint32_t new_timestamp,
                                    bool last_edge_cross_partition) = 0;
  virtual std::string get_name() = 0;

  virtual void assign_partition(std::vector<int32_t> &partitioning,
                        const std::set<uint32_t> &vertex_list, int32_t nparts);
  uint32_t
  calculate_movements_repartition(const std::vector<int32_t> &old_partitioning,
                                  const std::vector<int32_t> &new_partitioning,
                                  int32_t nparts);

  // Edges cut, vertices in each partitioning partitioning.size()
  std::tuple<uint32_t, std::vector<uint32_t>>
  calculate_edge_cut(const Graph &g, const std::vector<int32_t> &partitioning);
  void assign_partition_hash(std::vector<int32_t> &partitioning,
                             uint32_t vertex, int32_t nparts);
};