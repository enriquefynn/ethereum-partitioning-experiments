#include <iostream>
#include <partitioner.h>
#include <utils.h>
void Partitioner::assign_partition(const std::set<uint32_t> &vertex_list,
                                   int32_t nparts) {
  // std::cout << "Involved vertices: ";
  // for (const auto &it : vertex_list)
  //   std::cout << it << ' ';
  // std::cout << std::endl;
  std::vector<int32_t> best_partitions(nparts, 0);
  auto needs_partitioning = vertex_list.end();
  for (auto vertex = vertex_list.begin(); vertex != vertex_list.end();
       ++vertex) {
    if (*vertex >= m_partitioning.size()) {
      needs_partitioning = vertex;
      break;
    }
    ++best_partitions[m_partitioning[*vertex]];
  }

  int best_partition = 0;
  bool is_same = true;
  for (int i = 1; i < nparts; ++i) {
    if (best_partitions[i] != best_partitions[best_partition])
      is_same = false;
    if (best_partitions[i] > best_partitions[best_partition]) {
      best_partition = i;
    }
  }
  for (auto vertex = needs_partitioning; vertex != vertex_list.end();
       ++vertex) {
    //assert(*vertex == m_partitioning.size());
    // Cannot find good partition to put
    if (is_same) {
      auto most_unbalanced_partition =
          std::distance(m_balance.begin(),
                        std::min_element(m_balance.begin(), m_balance.end()));
      best_partition = most_unbalanced_partition;
    }
    m_partitioning[*vertex] = best_partition;
    ++m_balance[best_partition];
    // std::cout << "Assigning part to: " << *vertex << " in " << best_partition
    //           << " size: " << m_partitioning.size() << std::endl;
  }
}

const std::tuple<uint32_t, std::vector<uint32_t>>
Partitioner::calculate_edge_cut(const Graph &g) {
  typename GraphTraits::edge_iterator ei, ei_end;
  uint32_t edges_cut = 0;

  for (int i = 0; i < m_config.N_PARTITIONS; ++i)
    m_balance[i] = 0;
  for (auto vertex = boost::vertices(g); vertex.first != vertex.second;
       ++vertex.first) {
    auto vertex_id = Utils::get_id(*vertex.first, m_graph);
    ++m_balance[m_partitioning[vertex_id]];
  }

  for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
    auto source_id = Utils::get_id(boost::source(*ei, g), m_graph);
    auto target_id = Utils::get_id(boost::target(*ei, g), m_graph);
    if (m_partitioning[source_id] != m_partitioning[target_id])
      ++edges_cut;
  }

  return make_tuple(edges_cut, m_balance);
}
