#pragma once

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

  const uint8_t PARTITIONING_MODE = DYNAMIC_PARTITIONING;

  std::vector<uint32_t> get_neighbors(const std::vector<int32_t> &partitioning);

public:
  FB_partitioner(const Graph &graph) : Partitioner(0, graph) {}
  // void assign_partition(std::vector<int32_t> &partitioning,
  //                       const std::set<uint32_t> &vertex_list, int32_t nparts);

  std::vector<int32_t> partition(int32_t nparts) {
    return std::vector<int32_t>(boost::num_vertices(m_graph), 0);
  };

  bool trigger_partitioning(uint32_t new_timestamp,
                            bool last_edge_cross_partition);

  std::string get_name() { return "FACEBOOK"; }
};