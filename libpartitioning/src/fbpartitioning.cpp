#include <algorithm>
#include <iostream>
#include <iterator>

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
FB_partitioner::get_neighbors() {

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

      ++neighbors_in_partition[m_partitioning[boost::target(ed, m_graph)]];
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

uint32_t FB_partitioner::partition(int32_t nparts) {
  auto where_to_go = get_neighbors();
  



  m_partitioning = std::vector<int32_t>(boost::num_vertices(m_graph), 0);


  auto old_partitioning = std::move(m_partitioning);
  return calculate_movements_repartition(old_partitioning, nparts);
  
};

// Hash partitioning for new vertexes
void FB_partitioner::assign_partition(const std::set<uint32_t> &vertex_list,
                                      int32_t nparts) {
  auto needs_partitioning = vertex_list.end();
  uint32_t best_partition;
  for (auto vertex = vertex_list.begin(); vertex != vertex_list.end();
       ++vertex) {
    if (*vertex >= m_partitioning.size()) {
      needs_partitioning = vertex;
      break;
    }
  }

  for (auto vertex = needs_partitioning; vertex != vertex_list.end();
       ++vertex) {
    best_partition = (*vertex % nparts);
    assert(*vertex == m_partitioning.size());
    // Cannot find good partition to put
    m_partitioning.push_back(best_partition);
    ++m_balance[best_partition];
  }
}

std::string FB_partitioner::get_name() {
  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << CROSS_PARTITION_THRESHOLD;
  std::string threshold = stream.str();
  std::string partitioning_mode =
      (PARTITIONING_MODE == PERIODIC_PARTITIONING)
          ? "PERIODIC_"
          : "DYNAMIC_" + threshold + "_WINDOW_" +
                std::to_string(TIME_REPARTITION_WINDOW) + "_";

  return "FACEBOOK_" + partitioning_mode + "repart_" + std::to_string(TIME_REPARTITION) + "_seed_" +
         std::to_string(m_seed);
}
