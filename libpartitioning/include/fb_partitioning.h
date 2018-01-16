#pragma once

#include <iomanip>
#include <random>
#include <vector>

#include <config.h>
#include <partitioner.h>

#define FACEBOOK_SEED 0

class FB_partitioner : public Partitioner {
  std::unordered_map<uint32_t, uint32_t> get_neighbors(uint32_t n_partitions);
  std::vector<std::vector<std::vector<uint32_t>>> m_partition_vtx_to_move;

  std::mt19937 m_gen;
  std::uniform_real_distribution<> m_dis;

public:
  std::vector<std::vector<double>> get_oracle_matrix(uint32_t n_partitions);

  FB_partitioner(Graph &graph, Config &config);
  void assign_partition(const std::set<uint32_t> &vertex_list, int32_t nparts);

  uint32_t partition(int32_t n_partitions);

  std::string get_name();
};