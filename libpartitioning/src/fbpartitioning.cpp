#include <algorithm>
#include <iostream>
#include <iterator>

#include <fbpartitioning.h>
#include <utils.h>

FB_partitioner::FB_partitioner(const Graph &graph, Config &config)
    : Partitioner(FACEBOOK_SEED, graph, config), m_gen(FACEBOOK_SEED) {}

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

std::unordered_map<uint32_t, uint32_t>
FB_partitioner::get_neighbors(uint32_t n_partitions) {

  out_edge_it edg_it, edg_it_end;
  Edge ed;
  std::unordered_map<uint32_t, uint32_t> partition_to_go(boost::num_vertices(m_graph));
  // For every vertex, calcuate neighbors other partitions
  for (auto vertex = boost::vertices(m_graph); vertex.first != vertex.second;
       ++vertex.first) {
    auto vertex_id = Utils::get_id(*vertex.first, m_graph);
    std::vector<uint32_t> neighbors_in_partition(n_partitions);
    // std::cout << boost::out_degree(*vertex.first, m_graph) << std::endl;

    for (tie(edg_it, edg_it_end) = boost::out_edges(*vertex.first, m_graph);
         edg_it != edg_it_end; ++edg_it) {
      ed = *edg_it;
      ++neighbors_in_partition[m_partitioning[Utils::get_id(
          boost::target(ed, m_graph), m_graph)]];
    }
    // std::cout << "Neigh of " << *vertex.first << ": ";
    // for (int i = 0; i < neighbors_in_partition.size(); ++i) {
    //   std::cout << neighbors_in_partition[i] << ' ';
    // }
    // std::cout << std::endl;
    uint32_t max_p = m_partitioning[vertex_id];
    for (int i = 0; i < n_partitions; ++i) {
      if (neighbors_in_partition[i] > neighbors_in_partition[max_p]) {
        max_p = i;
      }
    }
    // std::cout << "max v[" << *vertex.first << "]: " << max_p << std::endl;
    partition_to_go[vertex_id] = max_p;
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

  m_partition_vtx_to_move = std::vector<std::vector<std::vector<uint32_t>>>(
      n_partitions, std::vector<std::vector<uint32_t>>(n_partitions));

  auto oracle_matrix = std::vector<std::vector<double>>(
      n_partitions, std::vector<double>(n_partitions, 0.));

  for (auto vertex = boost::vertices(m_graph); vertex.first != vertex.second;
       ++vertex.first) {
    auto vertex_id = Utils::get_id(*vertex.first, m_graph);
    // for (uint32_t vtx = 0; vtx < where_vtx_go.size(); ++vtx) {
    if (m_partitioning[vertex_id] != where_vtx_go[vertex_id]) {
      ++oracle_matrix[m_partitioning[vertex_id]][where_vtx_go[vertex_id]];
      m_partition_vtx_to_move[m_partitioning[vertex_id]][where_vtx_go[vertex_id]].push_back(
          vertex_id);
      // std::cout << "want to go from " << m_partitioning[vtx] << " to " <<
      // where_vtx_go[vtx] << ": " << vtx << std::endl;
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
      } else {
        oracle_matrix[j][i] = oracle_matrix[i][j] / oracle_matrix[j][i];
        oracle_matrix[i][j] = 1;
      }
    }
  }

  return oracle_matrix;
}

uint32_t FB_partitioner::partition(int32_t n_partitions) {
  uint32_t n_moves = 0;
  auto oracle_matrix = get_oracle_matrix(n_partitions);
  for (int i = 0; i < n_partitions - 1; ++i) {
    for (int j = i + 1; j < n_partitions; ++j) {
      if (oracle_matrix[i][j] > 0) {
        // Move from i to j
        for (const auto v : m_partition_vtx_to_move[i][j]) {
          auto prob = m_dis(m_gen);
          auto should_go = prob <= oracle_matrix[i][j];
          if (should_go) {
            ++n_moves;
            // assert(m_partitioning[v] == 0);
            // std::cout << "prob: " << prob << " MOVE from: " <<
            // m_partitioning[v]
            //           << " to: " << j << std::endl;
            m_partitioning[v] = j;
          }
        }
      }
      // Move from j to i
      if (oracle_matrix[j][i] > 0) {
        for (const auto v : m_partition_vtx_to_move[j][i]) {
          auto prob = m_dis(m_gen);
          auto should_go = prob <= oracle_matrix[j][i];
          if (should_go) {
            ++n_moves;
            // assert(m_partitioning[v] == 0);
            // std::cout << "prob: " << prob << " MOVE from: " <<
            // m_partitioning[v]
            //           << " to: " << i << std::endl;
            m_partitioning[v] = i;
          }
        }
      }
    }
  }

  std::cout << "Oracle matrix: " << n_partitions << '\n';
  for (int i = 0; i < n_partitions; ++i) {
    for (int j = 0; j < n_partitions; ++j) {
      std::cout << oracle_matrix[i][j] << ' ';
    }
    std::cout << std::endl;
  }

  return n_moves;
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
