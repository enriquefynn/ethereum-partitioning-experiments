#pragma once
#include <cassert>
#include <iostream>
#include <metis.h>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include <config.h>
#include <partitioner.h>

class METIS : public Partitioner {
  uint32_t timestamp_last_repartition = 0;
  uint32_t timestamp_last_check = 0;
  float cross_partition_calls = 0;
  float total_calls = 0;

public:
  idx_t METIS_OPTIONS[METIS_NOPTIONS];
  METIS(idx_t seed, const Graph &graph, const Config &config);
  std::vector<idx_t> partition(idx_t nparts);

  bool trigger_partitioning(uint32_t new_timestamp,
                            bool last_edge_cross_partition);

  METIS &operator=(const METIS &) = delete;
  METIS(const METIS &) = delete;
};
