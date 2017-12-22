#pragma once

#include <chrono>
#include <config.h>
#include <set>
#include <utility>
#include <vector>

#include <partitioner.h>

#define CREATE_TYPE 0
#define CALL_TYPE 1
#define CALLCODE_TYPE 2
#define DELEGATECALL_TYPE 3
#define STATICCALL_TYPE 4
#define PRECOMPILED_TYPE 5
#define OPSELFDESTRUCT_TYPE 6

namespace Utils {
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
    // if (*vertex != partitioning.size())
    //   std::cout << *vertex << ' ' << partitioning.size() << std::endl;
    // assert(*vertex == partitioning.size());
    // Cannot find good partition to put
    partitioning[*vertex] = best_partition;
    ++balance[best_partition];
  }
}

inline bool has_value(int type) {
  return (type == CALL_TYPE) || (type == CREATE_TYPE) ||
         (type == CALLCODE_TYPE) || type == (OPSELFDESTRUCT_TYPE);
}
inline bool is_selfdestruct(int type) { return type == OPSELFDESTRUCT_TYPE; }

template <typename vertex_ds>
const uint32_t get_id(const vertex_ds &vd, const Graph &g) {
  auto vertex_index_map = get(boost::vertex_index, g);
  return get(vertex_index_map, vd);
}

template <typename vertex_id, typename weight_type, typename map_type>
std::pair<Edge, bool>
add_edge_or_update_weigth(const vertex_id &from, const vertex_id &to,
                          const weight_type &weight, Graph &g,
                          map_type &id_vertex_map) {

  auto vertex_index_map = get(boost::vertex_index, g);
  auto weights_map = get(boost::edge_weight, *g);

  auto v_fr = id_vertex_map.find(from);

  Vertex fr_desc, to_desc;
  if (v_fr == id_vertex_map.end()) {
    fr_desc = boost::add_vertex(g);
    put(vertex_index_map, fr_desc, from);
    id_vertex_map.insert(v_fr, typename map_type::value_type(from, fr_desc));
  } else
    fr_desc = (*v_fr).second;

  // Loop
  if (from == to) {
    Edge ed;
    return {ed, false};
  }
  auto v_to = id_vertex_map.find(to);
  if (v_to == id_vertex_map.end()) {
    to_desc = boost::add_vertex(g);
    put(vertex_index_map, to_desc, to);
    id_vertex_map.insert(v_to, typename map_type::value_type(to, to_desc));
  } else
    to_desc = (*v_to).second;

  return add_edge(fr_desc, to_desc, g);
}

template <typename vertex_id, typename map_type>
void remove_vertex(vertex_id from, Graph &g, Partitioner *p,
                   map_type &id_vertex_map) {
  auto v_fr = id_vertex_map.find(from);
  if (v_fr == id_vertex_map.end())
    return;
  assert(v_fr != id_vertex_map.end());
  auto delete_vertex = (*v_fr).second;
  p->remove_vertex(from);
  // std::cout << "REMOVE: " << delete_vertex << std::endl;
  boost::clear_vertex(delete_vertex, g);
  boost::remove_vertex(delete_vertex, g);
}

template <typename map_type>
void save_partitioning(map_type &partitioning,
                       uint32_t epoch, std::fstream &partitioning_file,
                       bool save_partitioning) {
  if (!save_partitioning)
    return;
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

}; // namespace Utils