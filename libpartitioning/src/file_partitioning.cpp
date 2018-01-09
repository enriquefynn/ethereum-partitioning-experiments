#include <algorithm>
#include <iostream>
#include <iterator>

#include <file_partitioning.h>
#include <utils.h>

File_partitioner::File_partitioner(const Graph &graph, Config &config)
    : Partitioner(0, graph, config), m_partitioning_file(config.FILE_PATH),
      m_partitioning_epoch(0) {
  assert(m_config.SAVE_PARTITIONING);

  uint32_t n_vertices, part, vertex;
  m_partitioning_file >> m_partitioning_epoch >> n_vertices;
  for (int i = 0; i < n_vertices; ++i) {
    m_partitioning_file >> vertex >> part;
    m_partitioning[vertex] = part;
  }
  m_partitioning_file >> m_partitioning_epoch;
}

bool File_partitioner::trigger_partitioning(
    uint32_t new_timestamp, uint32_t cross_edge_access,
    uint32_t same_partition_edge_access) {
  if (new_timestamp == m_partitioning_epoch) {
    return true;
  }
  return false;
}

uint32_t File_partitioner::partition(int32_t n_partitions) {

  uint32_t n_vertices, part, vertex;
  auto old_partitioning = std::move(m_partitioning);
  assert(m_partitioning.size() == 0);

  m_partitioning_file >> n_vertices;
  for (int i = 0; i < m_config.N_PARTITIONS; ++i)
    m_balance[i] = 0;
  for (int i = 0; i < n_vertices; ++i) {
    m_partitioning_file >> vertex >> part;
    m_partitioning[vertex] = part;
    ++m_balance[part];
  }
  m_partitioning_file >> m_partitioning_epoch;
  return 0;
  // return calculate_movements_repartition(old_partitioning, n_partitions);
}

const std::tuple<uint32_t, std::vector<uint32_t>>
  File_partitioner::calculate_edge_cut(const Graph &g) {
  typename GraphTraits::edge_iterator ei, ei_end;
  uint32_t edges_cut = 0;

  for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
    auto source_id = Utils::get_id(boost::source(*ei, g), m_graph);
    auto target_id = Utils::get_id(boost::target(*ei, g), m_graph);
    if (m_partitioning[source_id] != m_partitioning[target_id])
      ++edges_cut;
  }
  return {edges_cut, m_balance};
  }
