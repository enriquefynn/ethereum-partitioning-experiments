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
  uint32_t timestamp_last_repartition = 0;
  uint32_t timestamp_last_check = 0;
  float cross_partition_calls = 0;
  float total_calls = 0;

  const uint32_t TIME_REPARTITION = 60 * 60 * 24 * 15;       // 15 days
  const uint32_t TIME_REPARTITION_WINDOW = 60 * 60 * 24 * 2; // 2 days
  const float CROSS_PARTITION_THRESHOLD =
      0.1; // Threshold for when trigger repartition

  const uint8_t PARTITIONING_MODE = PERIODIC_PARTITIONING;

  std::map<std::set<uint32_t>, uint32_t> m_hGraph;
  std::set<uint32_t> m_deleted_vertices;
  int m_eind_size = 0;

public:
  const int METIS_UBFACTOR = 15;
  int METIS_OPTIONS[9];
  HMETIS_partitioner(const Graph &graph);
  uint32_t partition(int nparts);

  bool trigger_partitioning(uint32_t new_timestamp, uint32_t cross_edge_access,
                            uint32_t same_partition_edge_access);

  void assign_partition(const std::set<uint32_t> &vertex_list, int32_t nparts);
  void remove_vertex(uint32_t vtx);

  std::string get_name();

  HMETIS_partitioner &operator=(const HMETIS_partitioner &) = delete;
  HMETIS_partitioner(const HMETIS_partitioner &) = delete;
};