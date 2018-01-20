#include <hash_partitioning.h>
#include <utils.h>
void Hash_partitioner::assign_partition(const std::set<uint32_t> &vertex_list,
                                        int32_t nparts) {
  Utils::assign_hash_partition(m_partitioning, m_balances, vertex_list, nparts);
}