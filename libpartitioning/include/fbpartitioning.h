#pragma once

#include <iomanip>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include <config.h>
#include <partitioner.h>

class FB_partitioner : public Partitioner {

  uint32_t timestamp_last_repartition = 0;
  uint32_t timestamp_last_check = 0;
  float cross_partition_calls = 0;
  float total_calls = 0;

  const uint32_t TIME_REPARTITION = 60 * 60 * 24 * 15;       // 15 days
  const uint32_t TIME_REPARTITION_WINDOW = 60 * 60 * 24 * 2; // 2 days
  const float CROSS_PARTITION_THRESHOLD =
      0.3; // Threshold for when trigger repartition

  const uint8_t PARTITIONING_MODE = PERIODIC_PARTITIONING;

  std::vector<uint32_t> get_neighbors(uint32_t n_partitions);
  
public:
  std::vector<std::vector<double>>
  get_oracle_matrix(uint32_t n_partitions);


  FB_partitioner(const Graph &graph);
  void assign_partition(const std::set<uint32_t> &vertex_list, int32_t nparts);

  uint32_t partition(int32_t n_partitions);

  bool trigger_partitioning(uint32_t new_timestamp, uint32_t cross_edge_access,
                            uint32_t same_partition_edge_access);

  std::string get_name();
};