#include <cassert>
#include <iomanip>
#include <metis.h>

#include <future_partitioning.h>
#include <log.h>
#include <partitioner.h>
#include <utils.h>

void Future_partitioner::assign_partition(const std::set<uint32_t> &vertex_list,
                                          int32_t nparts) {
  Utils::assign_hash_partition(m_partitioning, m_balances, vertex_list, nparts);
}

uint32_t Future_partitioner::partition(int32_t nparts) {
  uint32_t moves = METIS_partitioner::partition(nparts);

  auto before = std::chrono::high_resolution_clock::now();

  Utils::save_partitioning(m_partitioning, m_last_partitioning_time, m_config.FILE_INPUT);

  auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            (std::chrono::high_resolution_clock::now() - before))
            .count();
  LOG_DEBUG("Time to save partitioning: %lld", now);

  m_graph.clear();
  m_id_to_vertex.clear();
  m_partitioning.clear();
  return moves;
}