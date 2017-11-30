#pragma once
#include <cassert>
#include <iostream>
#include <metis.h>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include <config.h>
#include <partitioner.h>

#define METIS_SEED 1

class METIS_partitioner : public Partitioner {
  uint32_t timestamp_last_repartition = 0;
  uint32_t timestamp_last_check = 0;
  float cross_partition_calls = 0;
  float total_calls = 0;

  const uint32_t TIME_REPARTITION = 60 * 60 * 24 * 15; // 15 days
  const uint32_t TIME_REPARTITION_WINDOW = 60 * 60 * 24 * 2; // 2 days
  const float CROSS_PARTITION_THRESHOLD =
      0.1; // Threshold for when trigger repartition

  const uint8_t PARTITIONING_MODE = PERIODIC_PARTITIONING;

public:
  idx_t METIS_OPTIONS[METIS_NOPTIONS];
  METIS_partitioner(const Graph &graph);
  uint32_t partition(idx_t nparts);

  bool trigger_partitioning(uint32_t new_timestamp,
                            bool last_edge_cross_partition);

  std::string get_name();

  METIS_partitioner &operator=(const METIS_partitioner &) = delete;
  METIS_partitioner(const METIS_partitioner &) = delete;
};