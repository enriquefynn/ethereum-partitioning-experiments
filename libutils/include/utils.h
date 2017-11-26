#include <vector>
#include <metis.h>
#pragma once

namespace Utils {
void assign_hash_parititon(std::vector<idx_t> &partitioning,
                           uint32_t from_vertex, uint32_t to_vertex,
                           uint32_t nparts);
};