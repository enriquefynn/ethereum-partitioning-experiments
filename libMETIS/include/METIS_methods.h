#pragma once
#include <cassert>
#include <iostream>
#include <metis.h>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include <config.h>

class METIS {
  const Config &config;
  uint32_t timestamp_last_repartition = 0;
  uint32_t timestamp_last_check = 0;
  float cross_partition_calls = 0;
  float total_calls = 0;

public:
  idx_t METIS_OPTIONS[METIS_NOPTIONS];
  METIS(idx_t seed, const Config &_config);
  std::vector<idx_t> partition_METIS(const Graph &g, idx_t nparts);
  uint32_t
  calculate_movements_repartition(const std::vector<idx_t> &old_partitioning,
                                  const std::vector<idx_t> &new_partitioning,
                                  idx_t nparts);

  void assign_partition_hash(std::vector<idx_t> &partitioning, uint32_t vertex,
                             idx_t nparts);
  void assign_partition_same(std::vector<idx_t> &partitioning,
                             uint32_t from_vertex, uint32_t to_vertex,
                             idx_t nparts);

  bool trigger_partitioning(uint32_t new_timestamp,
                            bool last_edge_cross_partition);

  // Edges cut, vertices in each partitioning partitioning.size()
  std::tuple<uint32_t, std::vector<uint32_t>>
  calculate_edge_cut(const Graph &g, const std::vector<idx_t> &partitioning);

  METIS &operator=(const METIS &) = delete;
  METIS(const METIS &) = delete;
};
