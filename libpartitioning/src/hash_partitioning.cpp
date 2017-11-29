#include <hash_partitioning.h>

void Hash_partitioner::assign_partition(std::vector<int32_t> &partitioning,
                                        const std::set<uint32_t> &vertex_list,
                                        int32_t nparts) {
  auto needs_partitioning = vertex_list.end();
  uint32_t best_partition;
  for (auto vertex = vertex_list.begin(); vertex != vertex_list.end();
       ++vertex) {
    if (*vertex >= partitioning.size()) {
      needs_partitioning = vertex;
      break;
    }
  }

  for (auto vertex = needs_partitioning; vertex != vertex_list.end();
       ++vertex) {
    best_partition = (*vertex % nparts);
    assert(*vertex == partitioning.size());
    // Cannot find good partition to put
    partitioning.push_back(best_partition);
    ++m_balance[best_partition];
  }
}