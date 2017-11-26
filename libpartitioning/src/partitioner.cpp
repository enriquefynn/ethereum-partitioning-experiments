#include <partitioner.h>

void Partitioner::assign_partition_same(std::vector<int32_t> &partitioning,
                                        uint32_t from_vertex,
                                        uint32_t to_vertex, int32_t nparts) {
  // NO partition for from_vertex
  if (from_vertex == partitioning.size()) {
    // Self-reference
    if (to_vertex == from_vertex) {
      partitioning.push_back(from_vertex % nparts);
    }
    // No partition for both vertices
    else if (to_vertex >= partitioning.size()) {
      partitioning.push_back(from_vertex % nparts);
      partitioning.push_back(to_vertex % nparts);
    }
    // Partitioning to to_vertex exist
    else {
      partitioning.push_back(partitioning[to_vertex]);
    }
  }
  // to_vertex has no partition
  else if (to_vertex == partitioning.size()) {
    if (from_vertex == to_vertex) {
      partitioning.push_back(from_vertex % nparts);
    }
    // No partition for both vertices
    else if (from_vertex > partitioning.size()) {
      partitioning.push_back(from_vertex % nparts);
      partitioning.push_back(to_vertex % nparts);
    }
    // Partitioning to to_vertex exist
    else {
      partitioning.push_back(partitioning[from_vertex]);
    }
  }
}

uint32_t Partitioner::calculate_movements_repartition(
    const std::vector<int32_t> &old_partitioning,
    const std::vector<int32_t> &new_partitioning, int32_t nparts) {
  assert(old_partitioning.size() == new_partitioning.size());

  uint32_t moves = 0;
  const size_t new_partitioning_size = new_partitioning.size();
  const size_t old_partitioning_size = old_partitioning.size();

  for (size_t i = 0; i < old_partitioning_size; ++i)
    if (old_partitioning[i] != new_partitioning[i])
      ++moves;
  for (size_t i = old_partitioning_size; i < new_partitioning_size; ++i)
    if (static_cast<int32_t>(i % nparts) != new_partitioning[i])
      ++moves;
  return moves;
}

std::tuple<uint32_t, std::vector<uint32_t>>
Partitioner::calculate_edge_cut(const Graph &g,
                                const std::vector<int32_t> &partitioning) {
  typename GraphTraits::edge_iterator ei, ei_end;
  uint32_t edges_cut = 0;

  std::vector<uint32_t> balances(N_PARTITIONS);
  for (auto vertex = boost::vertices(g); vertex.first != vertex.second;
       ++vertex.first)
    ++balances[partitioning[*vertex.first]];

  for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    if (partitioning[boost::source(*ei, g)] !=
        partitioning[boost::target(*ei, g)])
      ++edges_cut;

  return make_tuple(edges_cut, balances);
}

void Partitioner::assign_partition_hash(std::vector<int32_t> &partitioning,
                                  uint32_t vertex, int32_t nparts) {
  if (vertex == partitioning.size()) {
    partitioning.push_back(vertex % nparts);
  }
}
