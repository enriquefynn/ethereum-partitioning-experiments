#include <algorithm>
#include <iterator>
#include <iostream>

#include <fbpartitioning.h>

bool FB_partitioner::trigger_partitioning(uint32_t new_timestamp,
                                 bool last_edge_cross_partition) {
  if (PARTITIONING_MODE == DYNAMIC_PARTITIONING) {
    ++total_calls;
    cross_partition_calls += last_edge_cross_partition;
    if (new_timestamp - timestamp_last_check > TIME_REPARTITION_WINDOW) {

      if (new_timestamp - timestamp_last_repartition > TIME_REPARTITION) {
        if ((cross_partition_calls / total_calls) > CROSS_PARTITION_THRESHOLD) {
          timestamp_last_repartition = new_timestamp;
          return true;
        }
      }

      cross_partition_calls = 0;
      total_calls = 0;
      timestamp_last_check = new_timestamp;
    }
    return false;
  } else if (PARTITIONING_MODE == PERIODIC_PARTITIONING) {
    if (new_timestamp - timestamp_last_repartition > TIME_REPARTITION) {
      timestamp_last_repartition = new_timestamp;
      return true;
    }
    return false;
  } else
    assert(false);
}

std::vector<uint32_t>
FB_partitioner::get_neighbors(const std::vector<int32_t> &partitioning) {

  out_edge_it edg_it, edg_it_end;
  Edge ed;
  std::vector<uint32_t> partition_to_go(boost::num_vertices(m_graph));
  // For every vertex, calcuate neighbors other partitions
  for (auto vertex = boost::vertices(m_graph); vertex.first != vertex.second;
       ++vertex.first) {
    std::vector<uint32_t> neighbors_in_partition(N_PARTITIONS);
    // std::cout << boost::out_degree(*vertex.first, m_graph) << std::endl;

    for (tie(edg_it, edg_it_end) = boost::out_edges(*vertex.first, m_graph);
         edg_it != edg_it_end; ++edg_it) {
      ed = *edg_it;

      ++neighbors_in_partition[partitioning[boost::target(ed, m_graph)]];
    }
    auto max_p = std::distance(neighbors_in_partition.begin(),
                               std::max_element(neighbors_in_partition.begin(),
                                                neighbors_in_partition.end()));
    partition_to_go[*vertex.first] = max_p;

    // for (const auto &it : neighbors_in_partition)
    //   std::cout << it << ' ';
    // std::cout << std::endl;
  }
  return partition_to_go;
}

