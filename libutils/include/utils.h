#pragma once

#include <chrono>
#include <set>
#include <utility>
#include <vector>

#include <config.h>
#include <partitioner.h>

namespace Utils {
enum TUPLE_PROP { EDGE, PROP };
enum EDGE_PROP { FOUND, NOT_FOUND, INVALID };

template <typename map_type>
void assign_hash_partition(map_type &partitioning,
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
    partitioning[*vertex] = best_partition;
    ++balance[best_partition];
  }
}

inline bool has_value(int type) {
  return (type == Config::CALL_TYPE) || (type == Config::CREATE_TYPE) ||
         (type == Config::CALLCODE_TYPE) ||
         type == (Config::OPSELFDESTRUCT_TYPE);
}
inline bool is_selfdestruct(int type) {
  return type == Config::OPSELFDESTRUCT_TYPE;
}

template <typename vertex_ds>
const uint32_t get_id(const vertex_ds &vd, const Graph &g) {
  return g[vd].m_vertex_id;
  // auto vertex_index_map = get(boost::vertex_index, g);
  // return get(vertex_index_map, vd);
}

template <typename vertex_id, typename weight_type>
std::tuple<Edge, Utils::EDGE_PROP>
add_edge_or_update_weigth(const vertex_id from, const vertex_id to,
                          const weight_type weight, Graph &g,
                          Partitioner &partitioner) {
  // auto vertex_index_map = get(boost::vertex_index, g);
  auto weights_map = get(boost::edge_weight, g);

  auto v_fr = partitioner.m_id_to_vertex.find(from);
  Vertex fr_desc, to_desc;
  if (v_fr == std::end(partitioner.m_id_to_vertex)) {
    fr_desc = boost::add_vertex(g);
    g[fr_desc] = VertexProperty(from);
    // put(vertex_index_map, fr_desc, from);
    partitioner.m_id_to_vertex.insert(v_fr, {from, fr_desc});
  } else
    fr_desc = (*v_fr).second;

  // Loop
  if (from == to) {
    Edge ed;
    return {ed, INVALID};
  }
  auto v_to = partitioner.m_id_to_vertex.find(to);
  if (v_to == std::end(partitioner.m_id_to_vertex)) {
    to_desc = boost::add_vertex(g);
    g[to_desc] = VertexProperty(to);
    // put(vertex_index_map, to_desc, to);
    partitioner.m_id_to_vertex.insert(v_to, {to, to_desc});
  } else
    to_desc = (*v_to).second;

  // Update edge weight
  Edge edge;
  bool edge_found = false;
  tie(edge, edge_found) = boost::edge(fr_desc, to_desc, g);
  if (edge_found) {
    auto current_weight = get(weights_map, edge);
    put(weights_map, edge, current_weight + weight);
  } else {
    tie(edge, std::ignore) = std::move(boost::add_edge(fr_desc, to_desc, g));
    put(weights_map, edge, weight);
  }
  // Update vertex weight
  ++g[boost::source(edge, g)].m_vertex_weight;
  ++g[boost::target(edge, g)].m_vertex_weight;

  partitioner.added_edge(from, to);
  return {edge, edge_found ? FOUND : NOT_FOUND};
}

template <typename map_type>
void save_partitioning(map_type &partitioning, uint32_t epoch,
                       std::fstream &partitioning_file) {

  partitioning_file << epoch << ' ' << partitioning.size() << std::endl;
  for (const auto &p : partitioning)
    partitioning_file << p.first << ' ' << p.second << std::endl;
}

template <typename F, typename... Args>
double measure_time(F func, Args &&... args) {
  auto before = std::chrono::high_resolution_clock::now();
  func(std::forward<Args>(args)...);
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             (std::chrono::high_resolution_clock::now() - before))
      .count();
}

void LOG_REPARTITION(std::ofstream &stats_file, const Graph &graph,
                     uint32_t timestamp, uint32_t movements_to_repartition,
                     uint32_t edges_cut, const std::vector<uint32_t> &balance);

void LOG_POINT(std::ofstream &stats_file, const Graph &graph,
               uint32_t cross_partition_tx_access,
               uint32_t same_partition_tx_access, uint32_t new_timestamp,
               const std::vector<uint32_t> &txs_per_partition,
               const std::unique_ptr<Partitioner> &partitioner,
               uint32_t edges_cut);

// const std::unordered_set<uint32_t> pre_compiled_contracts = {
//     17596,  // 1 ecrecover
//     30877,  // 2 sha256hash
//     320152, // 3 ripemd160hash
//     9554,   // 4 dataCopy
//     698904, // 5 bigModExp
//     698905, // 6 bn256Add
//     698906, // 7 bn256ScalarMul
//     698907, // 8 bn256Pairing
// };

inline bool is_precompiled(uint32_t tx_type) {
  return tx_type == Config::PRECOMPILED_TYPE;
}

}; // namespace Utils