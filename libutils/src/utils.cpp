#include <set>
#include <cassert>
#include "utils.h"

namespace Utils {
void assign_hash_partition(std::vector<int32_t> &partitioning,
                           std::vector<uint32_t> &balance,
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
    ++balance[best_partition];
  }
}
} // namespace Utils