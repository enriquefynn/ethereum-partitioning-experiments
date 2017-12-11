#pragma once

#include <config.h>
#include <set>
#include <vector>
#include <iostream>
class Partitioner {
protected:
  const int32_t m_seed;
  const Graph &m_graph;

public:
  std::vector<uint32_t> m_partitioning;
  std::vector<uint32_t> m_balance;
  Partitioner(int32_t seed, const Graph &graph)
      : m_seed(seed), m_graph(graph), m_partitioning(0),
        m_balance(N_PARTITIONS, 0){};

  virtual uint32_t partition(int32_t n_part) = 0;
  virtual bool trigger_partitioning(uint32_t new_timestamp,
                                    uint32_t cross_edge_access,
                                    uint32_t same_partition_edge_access) = 0;
  virtual std::string get_name() = 0;

  virtual void assign_partition(const std::set<uint32_t> &vertex_list,
                                int32_t nparts);

  virtual void define_partitioning(std::vector<uint32_t> &&new_partitioning) {
    assert(new_partitioning.size() == boost::num_vertices(m_graph));
    m_partitioning = std::move(new_partitioning);
  }

  virtual void remove_vertex(uint32_t vtx) {
    assert(vtx < m_partitioning.size());
    assert(m_partitioning[vtx] < N_PARTITIONS);
    assert(m_balance[m_partitioning[vtx]] > 0);
    --m_balance[m_partitioning[vtx]];
    m_partitioning[vtx] = N_PARTITIONS;
  }

  const uint32_t
  calculate_movements_repartition(const std::vector<uint32_t> &old_partitioning,
                                  int32_t nparts) const;

  // Edges cut, vertices in each partitioning partitioning.size()
  const std::tuple<uint32_t, std::vector<uint32_t>>
  calculate_edge_cut(const Graph &g);

  inline bool same_partition(uint32_t v1, uint32_t v2) const {
    return m_partitioning[v1] == m_partitioning[v2];
  }
};