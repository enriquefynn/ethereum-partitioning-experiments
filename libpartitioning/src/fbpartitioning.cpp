#include <algorithm>
#include <iostream>
#include <iterator>

#include <fbpartitioning.h>
#include <utils.h>

FB_partitioner::FB_partitioner(const Graph &graph) : Partitioner(0, graph) {}

bool FB_partitioner::trigger_partitioning(uint32_t new_timestamp,
                                          uint32_t cross_edge_access,
                                          uint32_t same_partition_edge_access) {
  if (PARTITIONING_MODE == DYNAMIC_PARTITIONING) {
    cross_partition_calls += static_cast<float>(cross_edge_access);
    total_calls +=
        static_cast<float>(cross_edge_access + same_partition_edge_access);
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
  } else {
    assert(false);
  }
}

std::vector<uint32_t> FB_partitioner::get_neighbors(uint32_t n_partitions) {

  out_edge_it edg_it, edg_it_end;
  Edge ed;
  std::vector<uint32_t> partition_to_go(boost::num_vertices(m_graph));
  // For every vertex, calcuate neighbors other partitions
  for (auto vertex = boost::vertices(m_graph); vertex.first != vertex.second;
       ++vertex.first) {
    std::vector<uint32_t> neighbors_in_partition(n_partitions);
    // std::cout << boost::out_degree(*vertex.first, m_graph) << std::endl;

    for (tie(edg_it, edg_it_end) = boost::out_edges(*vertex.first, m_graph);
         edg_it != edg_it_end; ++edg_it) {
      ed = *edg_it;
      ++neighbors_in_partition[m_partitioning[boost::target(ed, m_graph)]];
    }
    // std::cout << "Neigh of " << *vertex.first << ": ";
    // for (int i = 0; i < neighbors_in_partition.size(); ++i) {
    //   std::cout << neighbors_in_partition[i] << ' ';
    // }
    // std::cout << std::endl;
    uint32_t max_p = m_partitioning[*vertex.first];
    for (int i = 0; i < n_partitions; ++i) {
      if (neighbors_in_partition[i] > neighbors_in_partition[max_p]) {
        max_p = i;
      }
    }
    // std::cout << "max v[" << *vertex.first << "]: " << max_p << std::endl;
    partition_to_go[*vertex.first] = max_p;
  }

  // for (auto vertex = boost::vertices(m_graph); vertex.first != vertex.second;
  //      ++vertex.first) {
  // std::cout << "V" << *vertex.first << ": ";
  //   for (tie(edg_it, edg_it_end) = boost::out_edges(*vertex.first, m_graph);
  //        edg_it != edg_it_end; ++edg_it) {
  //     ed = *edg_it;
  //     std::cout << boost::target(ed, m_graph) << ' ';
  //   }
  //   std::cout << std::endl;
  // }

  // for (int i = 0; i < m_partitioning.size(); ++i) {
  //   std::cout << m_partitioning[i] << ' ';
  // }
  // std::cout << std::endl;
  // std::cout << "partitions to go: ";
  // for (const auto &it : partition_to_go) {
  //   std::cout << it << ' ';
  // }
  // std::cout << std::endl;
  // exit(0);
  return partition_to_go;
}

std::vector<std::vector<double>>
FB_partitioner::get_oracle_matrix(uint32_t n_partitions) {
  auto where_vtx_go = get_neighbors(n_partitions);
  auto oracle_matrix = std::vector<std::vector<double>>(
      n_partitions, std::vector<double>(n_partitions, 0.));

  for (uint32_t vtx = 0; vtx < where_vtx_go.size(); ++vtx) {
    if (m_partitioning[vtx] != where_vtx_go[vtx]) {
      ++oracle_matrix[m_partitioning[vtx]][where_vtx_go[vtx]];
      // partitions_go_to[m_partitioning[vtx]].push_back(where_vtx_go[vtx]);
    }
  }

  for (int i = 0; i < n_partitions - 1; ++i) {
    for (int j = i + 1; j < n_partitions; ++j) {
      bool min_idx = false;
      if (!oracle_matrix[i][j] || !oracle_matrix[j][i])
        continue;
      if (oracle_matrix[i][j] < oracle_matrix[j][i]) {
        min_idx = true;
      }
      if (min_idx == false) {
        oracle_matrix[i][j] = oracle_matrix[j][i] / oracle_matrix[i][j];
        oracle_matrix[j][i] = 1;
      }
      else {
        oracle_matrix[j][i] = oracle_matrix[i][j] / oracle_matrix[j][i];
        oracle_matrix[i][j] = 1;
      }
    }
  }

  return oracle_matrix;
}

uint32_t FB_partitioner::partition(int32_t n_partitions) {
  // std::vector<std::vector<uint32_t>> partitions_go_to(N_PARTITIONS,
  //                                                     std::vector<uint32_t>());

  // Build matrix
  // uint32_t oracle_matrix[N_PARTITIONS][N_PARTITIONS];
  // memset(oracle_matrix, 0, sizeof(uint32_t) * N_PARTITIONS * N_PARTITIONS);
  // for (uint32_t vtx = 0; vtx < where_vtx_go.size(); ++vtx) {
  //   if (m_partitioning[vtx] != where_vtx_go[vtx]) {
  //     ++oracle_matrix[m_partitioning[vtx]][where_vtx_go[vtx]];
  //     // partitions_go_to[m_partitioning[vtx]].push_back(where_vtx_go[vtx]);
  //   }
  // }

  // for (int i = 0; i < N_PARTITIONS; ++i) {
  //   std::cout << "PARTITION " << i << ": ";
  //   for (int j = 0; j < partitions_go_to[i].size(); ++j)

  // }

  // for (int i = 0; i < N_PARTITIONS; ++i) {
  //   std::cout << "PARTITION " << i << ": ";
  //   for (int j = 0; j < partitions_go_to[i].size(); ++j)
  //     std::cout << partitions_go_to[i][j] << ' ';
  //   std::cout << std::endl;
  // }

  get_oracle_matrix(n_partitions);
  return 0;
  // m_partitioning = std::vector<int32_t>(boost::num_vertices(m_graph), 0);
  // return 100;
  // auto old_partitioning = std::move(m_partitioning);
  // return calculate_movements_repartition(old_partitioning, nparts);
}

// Hash partitioning for new vertexes
void FB_partitioner::assign_partition(const std::set<uint32_t> &vertex_list,
                                      int32_t n_partitions) {
  Utils::assign_hash_partition(m_partitioning, m_balance, vertex_list,
                               n_partitions);
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

  return "FACEBOOK_" + partitioning_mode + "repart_" +
         std::to_string(TIME_REPARTITION) + "_seed_" + std::to_string(m_seed);
}
