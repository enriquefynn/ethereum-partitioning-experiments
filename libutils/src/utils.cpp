// #include <HMETIS_methods.h>
// #include <METIS_methods.h>
// #include <config.h>
// #include <fbpartitioning.h>
// #include <file_partitioning.h>
// #include <hash_partitioning.h>
// #include <log.h>
#include <utils.h>

namespace Utils {
void LOG_REPARTITION(std::ofstream &stats_file, const Graph &graph,
                     uint32_t timestamp, uint32_t movements_to_repartition,
                     uint32_t edges_cut, const std::vector<uint32_t> &balance) {
  stats_file << "REPARTITION " << timestamp << ' ' << boost::num_vertices(graph)
             << ' ' << boost::num_edges(graph) << ' '
             << movements_to_repartition << ' ' << edges_cut << ' ';
  for (int i = 0; i < balance.size(); ++i)
    stats_file << balance[i] << ' ';
  stats_file << std::endl;
}

void LOG_POINT(std::ofstream &stats_file, const Graph &graph,
               uint32_t cross_partition_tx_access,
               uint32_t same_partition_tx_access, uint32_t new_timestamp,
               const std::vector<uint32_t> &txs_per_partition,
               const std::unique_ptr<Partitioner> &partitioner) {
  stats_file << "POINT " << cross_partition_tx_access << ' '
             << same_partition_tx_access << ' ' << new_timestamp << ' ';
  for (int i = 0; i < partitioner->m_balances.size(); ++i)
    stats_file << partitioner->m_balances[i] << ' ' << txs_per_partition[i]
               << ' ';
  stats_file << std::endl;
}

} // namespace Utils