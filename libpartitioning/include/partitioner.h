#pragma once

#include <vector>

#include <config.h>

class Partitioner {
protected:
  const int32_t seed;
  const Graph &graph;
  const Config &config;

public:
  Partitioner(int32_t seed, const Graph &graph, const Config &config)
      : seed(seed), graph(graph), config(config){};

  virtual std::vector<int32_t> partition(int32_t n_part) = 0;
  virtual bool trigger_partitioning(uint32_t new_timestamp,
                                    bool last_edge_cross_partition) = 0;

  void assign_partition(std::vector<int32_t> &partitioning,
                             uint32_t from_vertex, uint32_t to_vertex,
                             int32_t nparts);
  uint32_t
  calculate_movements_repartition(const std::vector<int32_t> &old_partitioning,
                                  const std::vector<int32_t> &new_partitioning,
                                  int32_t nparts);

  // Edges cut, vertices in each partitioning partitioning.size()
  std::tuple<uint32_t, std::vector<uint32_t>>
  calculate_edge_cut(const Graph &g, const std::vector<int32_t> &partitioning);
  void assign_partition_hash(std::vector<int32_t> &partitioning, uint32_t vertex,
                             int32_t nparts);
};