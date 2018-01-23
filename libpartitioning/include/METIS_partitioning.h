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

public:
  idx_t METIS_OPTIONS[METIS_NOPTIONS];
  METIS_partitioner(Graph &graph, Config &config);
  uint32_t partition(const Graph &graph,
                     const std::map<uint32_t, Vertex> &id_to_vertex,
                     std::unordered_map<uint32_t, uint32_t> &partitioning,
                     idx_t nparts);
  uint32_t partition(idx_t nparts) {
    return partition(m_graph, m_id_to_vertex, m_partitioning, nparts);
  }
  std::string get_name();

  METIS_partitioner &operator=(const METIS_partitioner &) = delete;
  METIS_partitioner(const METIS_partitioner &) = delete;
};