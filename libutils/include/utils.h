#include <chrono>
#include <utility>
#include <vector>
#include <set>

#pragma once

namespace Utils {
void assign_hash_partition(std::vector<int32_t> &partitioning,
                           std::vector<uint32_t> &balance,
                           const std::set<uint32_t> &vertex_list,
                           int32_t nparts);

template <typename F, typename... Args>
double measure_time(F func, Args &&... args) {
  auto before = std::chrono::high_resolution_clock::now();
  func(std::forward<Args>(args)...);
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             (std::chrono::high_resolution_clock::now() - before))
      .count();
}

}; // namespace Utils