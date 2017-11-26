#include <hash_partitioning.h>

void Hash_partitioner::assign_partition(std::vector<int32_t> &partitioning,
                                        uint32_t from_vertex,
                                        uint32_t to_vertex, int32_t nparts) {
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
      // partitioning.push_back(partitioning[to_vertex]);
      partitioning.push_back(from_vertex % nparts);
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
    // Partitioning to from_vertex exist
    else {
      partitioning.push_back(to_vertex % nparts);
    }
  }
}