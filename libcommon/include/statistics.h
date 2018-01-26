#pragma once

#include <boost/graph/incremental_components.hpp>
#include <boost/pending/disjoint_sets.hpp>
#include <config.h>
#include <fstream>
#include <vector>

#include <log.h>
#include <partitioner.h>

class Statistics {
  const Graph &m_graph;
  const Partitioner &m_partitioner;
  uint32_t m_edges_cut = 0;

  uint32_t prev_n_nodes = 0;
  uint32_t prev_n_edges = 0;

  std::ofstream m_output_graph_size_evolution;
  std::ofstream m_output_graph_cc;
  bool m_log_graph_size = true;
  bool m_log_graph_cc = true;

  std::vector<uint32_t> m_rank;
  std::vector<uint32_t> m_parent;
  uint32_t m_previous_cc_count = 0;

  void log_graph_size(uint32_t timestamp, uint32_t n_nodes, uint32_t n_edges) {
    if (n_nodes != prev_n_nodes && n_edges != prev_n_edges) {
      m_output_graph_size_evolution << timestamp << ' ' << n_nodes << ' '
                                    << n_edges << std::endl;
      prev_n_nodes = n_nodes;
      prev_n_edges = n_edges;
    }
  }
  void log_connected_components(uint32_t timestamp) {
    if (m_previous_cc_count != m_number_of_cc) {
      m_output_graph_cc << timestamp << ' ' << m_number_of_cc << std::endl;
      m_previous_cc_count = m_number_of_cc;
    }
  };

  uint32_t p_find(uint32_t node);
  void p_union(uint32_t from, uint32_t to);

public:
  uint32_t m_number_of_cc;
  Statistics(const Graph &graph, const Partitioner &partitioner,
             const Config &config)
      : m_graph(graph), m_partitioner(partitioner),
        m_output_graph_size_evolution(config.GRAPH_SIZE_EVOLUTION_PATH),
        m_output_graph_cc(config.GRAPH_CC_PATH), m_number_of_cc(0) {

    if (!config.GRAPH_SIZE_EVOLUTION_PATH.size())
      m_log_graph_size = false;
    if (!config.GRAPH_CC_PATH.size())
      m_log_graph_cc = false;
  }
  void log(uint32_t timestamp);

  void add_edges(const std::vector<std::pair<uint32_t, uint32_t>> &edges);
  void define_edges_cut(uint32_t edges_cut) { m_edges_cut = edges_cut; }
};