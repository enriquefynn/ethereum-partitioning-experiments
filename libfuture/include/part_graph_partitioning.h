#pragma once

#include <METIS_partitioning.h>
#include <config.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

class Part_graph_partitioner : public METIS_partitioner {
private:
  Graph m_cur_graph;
  std::unordered_map<uint32_t, uint32_t> m_cur_partitioning;
  std::map<uint32_t, Vertex> m_cur_id_to_vertex;

public:
  Part_graph_partitioner(Graph &graph, Config &config)
      : METIS_partitioner(graph, config) {}

  uint32_t partition(int32_t nparts);
  std::string get_name();

  void terminate() {
    // partition(m_config.N_PARTITIONS);
    m_config.FILE_INPUT.close();
  }
  
  void added_edge(uint32_t from, uint32_t to);

  Part_graph_partitioner &operator=(const Part_graph_partitioner &) = delete;
  Part_graph_partitioner(const Part_graph_partitioner &) = delete;
};
