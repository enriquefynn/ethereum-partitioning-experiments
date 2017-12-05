#include "utils.h"
#include <cassert>
#include <set>

#include <iostream>
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

void add_edge_or_update_weigth(uint32_t from, uint32_t to, int weight, Graph &g) {
  boost::add_edge(from, to, 0, g);
  std::pair<Edge, bool> ed = boost::edge(from, to, g);
  uint32_t prev_weight = boost::get(boost::edge_weight_t(), g, ed.first);
  boost::put(boost::edge_weight_t(), g, ed.first, prev_weight + weight);
}
void remove_vertex(uint32_t from, Graph &g, Partitioner *p) {
  boost::clear_vertex(from, g);
  p->remove_vertex(from);
}

} // namespace Utils