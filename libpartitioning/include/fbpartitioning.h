#pragma once

#include <iomanip>
#include <random>
#include <vector>

#include <config.h>
#include <partitioner.h>

#define FACEBOOK_SEED 0

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
  std::unordered_map<uint32_t, uint32_t> get_neighbors(uint32_t n_partitions);
  std::vector<std::vector<std::vector<uint32_t>>> m_partition_vtx_to_move;

  std::mt19937 m_gen;
  std::uniform_real_distribution<> m_dis;

public:
  std::vector<std::vector<double>> get_oracle_matrix(uint32_t n_partitions);

  FB_partitioner(const Graph &graph, Config &config);
  void assign_partition(const std::set<uint32_t> &vertex_list, int32_t nparts);

  uint32_t partition(int32_t n_partitions);

  bool trigger_partitioning(uint32_t new_timestamp, uint32_t cross_edge_access,
                            uint32_t same_partition_edge_access);

  std::string get_name();
};