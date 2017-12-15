#pragma once

#include <chrono>
#include <set>
#include <utility>
#include <vector>
#include <config.h>

#include <partitioner.h>

#define CREATE_TYPE 0
#define CALL_TYPE 1
#define CALLCODE_TYPE 2
#define DELEGATECALL_TYPE 3
#define STATICCALL_TYPE 4
#define PRECOMPILED_TYPE 5
#define OPSELFDESTRUCT_TYPE 6

namespace Utils {
void assign_hash_partition(std::vector<uint32_t> &partitioning,
                           std::vector<uint32_t> &balance,
                           const std::set<uint32_t> &vertex_list,
                           int32_t nparts);
inline bool has_value(int type) {
  return (type == CALL_TYPE) || (type == CREATE_TYPE) ||
         (type == CALLCODE_TYPE) || type == (OPSELFDESTRUCT_TYPE);
}
inline bool is_selfdestruct(int type) { return type == OPSELFDESTRUCT_TYPE; }

void add_edge_or_update_weigth(uint32_t from, uint32_t to, int weight, Graph &g);
void remove_vertex(uint32_t from, Graph &g, Partitioner *p);

void save_partitioning(const std::vector<uint32_t> &partitioning,
                       uint32_t epoch, const std::string filename);

template <typename F, typename... Args>
double measure_time(F func, Args &&... args) {
  auto before = std::chrono::high_resolution_clock::now();
  func(std::forward<Args>(args)...);
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             (std::chrono::high_resolution_clock::now() - before))
      .count();
}

}; // namespace Utils