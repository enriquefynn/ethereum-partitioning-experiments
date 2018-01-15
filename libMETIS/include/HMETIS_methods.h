#pragma once
#include <cassert>
#include <iostream>
#include <map>
#include <metis.h>
#include <set>
#include <vector>

#include <boost/graph/graph_traits.hpp>

#include <config.h>
#include <partitioner.h>

#define METIS_SEED 1

extern "C" {
void HMETIS_PartKway(int nvtxs, int nhedges, int *vwgts, int *eptr, int *eind,
                     int *hewgts, int nparts, int ubfactor, int *options,
                     int *part, int *edgecut);

void HMETIS_PartRecursive(int nvtxs, int nhedges, int *vwgts, int *eptr,
                          int *eind, int *hewgts, int nparts, int ubfactor,
                          int *options, int *part, int *edgecut);
}

class HMETIS_partitioner : public Partitioner {
  std::unordered_map<uint32_t, uint32_t> m_saved_partitioning;

  std::map<std::set<uint32_t>, uint32_t> m_hGraph;
  std::set<uint32_t> m_deleted_vertices;
  int m_eind_size = 0;

public:
  const int METIS_UBFACTOR = 5;
  int METIS_OPTIONS[9];
  HMETIS_partitioner(const Graph &graph, Config &config);
  uint32_t partition(int nparts);

  void assign_partition(const std::set<uint32_t> &vertex_list, int32_t nparts);
  void remove_vertex(uint32_t vtx);

  std::string get_name();

  HMETIS_partitioner &operator=(const HMETIS_partitioner &) = delete;
  HMETIS_partitioner(const HMETIS_partitioner &) = delete;
};