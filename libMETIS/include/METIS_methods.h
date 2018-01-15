#pragma once
#include <cassert>
#include <iostream>
#include <metis.h>
#include <vector>

#include <boost/graph/graph_traits.hpp>

#include <config.h>
#include <partitioner.h>

#define METIS_SEED 1

class METIS_partitioner : public Partitioner {
  std::unordered_map<uint32_t, uint32_t> m_saved_partitioning;

public:
  idx_t METIS_OPTIONS[METIS_NOPTIONS];
  METIS_partitioner(const Graph &graph, Config &config);
  uint32_t partition(idx_t nparts);

  void assign_partition(const std::set<uint32_t> &vertex_list, int32_t nparts);

  std::string get_name();

  METIS_partitioner &operator=(const METIS_partitioner &) = delete;
  METIS_partitioner(const METIS_partitioner &) = delete;
};