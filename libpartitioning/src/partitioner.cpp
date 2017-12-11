#include <iostream>
#include <partitioner.h>
void Partitioner::assign_partition(const std::set<uint32_t> &vertex_list,
                                   int32_t nparts) {
  std::vector<int32_t> best_partitions(N_PARTITIONS, 0);
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
  for (int i = 1; i < N_PARTITIONS; ++i) {
    if (best_partitions[i] != best_partitions[best_partition])
      is_same = false;
    if (best_partitions[i] > best_partitions[best_partition]) {
      best_partition = i;
    }
  }
  for (auto vertex = needs_partitioning; vertex != vertex_list.end();
       ++vertex) {
    assert(*vertex == m_partitioning.size());
    // Cannot find good partition to put
    if (is_same) {
      auto most_unbalanced_partition =
          std::distance(m_balance.begin(),
                        std::min_element(m_balance.begin(), m_balance.end()));
      best_partition = most_unbalanced_partition;
    }
    m_partitioning.push_back(best_partition);
    ++m_balance[best_partition];
  }
}

const uint32_t Partitioner::calculate_movements_repartition(
    const std::vector<uint32_t> &old_partitioning, int32_t nparts) const {

  uint32_t moves = 0;
  const size_t new_partitioning_size = m_partitioning.size();
  const size_t old_partitioning_size = old_partitioning.size();
  assert(new_partitioning_size == old_partitioning_size);

  for (size_t i = 0; i < old_partitioning_size; ++i)
    if (old_partitioning[i] != m_partitioning[i])
      ++moves;
  return moves;
}

const std::tuple<uint32_t, std::vector<uint32_t>>
Partitioner::calculate_edge_cut(const Graph &g) {
  typename GraphTraits::edge_iterator ei, ei_end;
  uint32_t edges_cut = 0;

  for (int i = 0; i < N_PARTITIONS; ++i)
    m_balance[i] = 0;
  for (auto vertex = boost::vertices(g); vertex.first != vertex.second;
       ++vertex.first) {
    ++m_balance[m_partitioning[*vertex.first]];
  }

  for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    if (m_partitioning[boost::source(*ei, g)] !=
        m_partitioning[boost::target(*ei, g)])
      ++edges_cut;

  return make_tuple(edges_cut, m_balance);
}
