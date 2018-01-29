#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <config.h>
#include <helpers.h>
#include <metis.h>
#include <statistics.h>
#include <utils.h>

using std::string;
using std::set;
using std::cout;
using std::endl;
using std::vector;

int main(int argc, char **argv) {
  bool DEBUG = false;
  Graph graph;
  Config config = Config(argv[2]);
  cout << config;
  auto partitioner = GraphHelpers::get_partitioner(graph, config);
  Statistics statistics(graph, *partitioner, config);

  std::ifstream calls_file(argv[1]);
  std::ofstream stats_file("/tmp/edge_cut_evolution_partitions_" +
                           std::to_string(config.N_PARTITIONS) + "_period_" +
                           std::to_string(TIME_GAP_LOG) + "_" +
                           partitioner->get_name() + +".txt");

  uint32_t lineN, timestamp_log, new_timestamp;
  uint32_t from_vertex, to_vertex, weight;

  uint32_t block_number, has_from;
  uint32_t tx_type, tx_author, tx_type_n, tx_types_size;
  bool has_value, tx_failed;
  char header;
  string tx_value;

  // uint32_t cross_partition_edge_access, same_partition_edge_access;
  uint32_t cross_partition_tx_access, same_partition_tx_access;
  // same_partition_edge_access = cross_partition_edge_access = 0;
  cross_partition_tx_access = same_partition_tx_access = 0;

  lineN = timestamp_log = 0;

  set<uint32_t> involved_vertices;
  vector<std::tuple<Edge, Utils::EDGE_PROP>> involved_edges;
  vector<uint32_t> delete_vertices;

  // Genesis processing
  uint32_t n_transactions;
  calls_file >> header >> n_transactions;
  involved_vertices.insert(0);
  for (int tx = 0; tx < n_transactions; ++tx) {
    calls_file >> to_vertex >> tx_value;
    involved_vertices.insert(to_vertex);
    Utils::add_edge_or_update_weigth(0u, to_vertex, 1, graph, *partitioner);
  }
  partitioner->assign_partition(involved_vertices, config.N_PARTITIONS);
  // End Genesis processing

  // Track how many transactions touched the partition individually
  vector<uint32_t> txs_per_partition(config.N_PARTITIONS, 0);

  while (calls_file >> header >> block_number >> new_timestamp >>
         n_transactions) {
    assert(header == 'B');
    // cout << "BLOCK: " << block_number << endl;

    if (!timestamp_log)
      timestamp_log = new_timestamp;

    if (partitioner->trigger_partitioning(
            new_timestamp, cross_partition_tx_access, same_partition_tx_access,
            txs_per_partition)) {
      // Partition the graph
      auto before = std::chrono::high_resolution_clock::now();
      uint32_t movements_to_repartition =
          partitioner->partition(config.N_PARTITIONS);
      auto time_to_partition =
          std::chrono::duration_cast<std::chrono::milliseconds>(
              (std::chrono::high_resolution_clock::now() - before))
              .count();
      LOG_INFO("Partitioning took: %lldms", time_to_partition);
      // Calculate edge-cut / balance
      uint32_t edges_cut;

      vector<uint32_t> balance;
      tie(edges_cut, balance) = partitioner->calculate_edge_cut_balances(graph);
      statistics.define_edges_cut(edges_cut);
      Utils::LOG_REPARTITION(stats_file, graph, new_timestamp,
                             movements_to_repartition, edges_cut, balance);
    }

    assert(new_timestamp >= timestamp_log);
    if (new_timestamp - timestamp_log > TIME_GAP_LOG) {
      Utils::LOG_POINT(stats_file, graph, cross_partition_tx_access,
                       same_partition_tx_access, new_timestamp,
                       txs_per_partition, partitioner, statistics.m_edges_cut);

      same_partition_tx_access = cross_partition_tx_access = 0;
      std::fill(txs_per_partition.begin(), txs_per_partition.end(), 0);
      timestamp_log = new_timestamp;
    }

    for (int tx = 0; tx < n_transactions; ++tx) {
      calls_file >> header >> tx_author >> tx_failed >> tx_types_size;
      assert(header == 'T');

      involved_vertices.clear();
      involved_edges.clear();
      delete_vertices.clear();

      for (int tx_n_type = 0; tx_n_type < tx_types_size; ++tx_n_type) {
        calls_file >> tx_type >> tx_type_n;
        has_value = Utils::has_value(tx_type);
        for (int tx_call = 0; tx_call < tx_type_n; ++tx_call) {
          calls_file >> has_from;
          if (has_from == 1) {
            from_vertex = tx_author;
            calls_file >> to_vertex;
          } else if (has_from == 2) {
            calls_file >> from_vertex >> to_vertex;
          } else
            assert(false);
          if (has_value)
            calls_file >> tx_value;
          calls_file >> weight;
          if (Utils::is_precompiled(tx_type) && config.AVOID_PRECOMPILED)
            continue;
          involved_vertices.insert(from_vertex);
          involved_vertices.insert(to_vertex);
          if (Utils::is_selfdestruct(tx_type) && !tx_failed) {
            // From: contract that was suicided
            // To: Funds moved there
            delete_vertices.push_back(from_vertex);
          }
          involved_edges.push_back(Utils::add_edge_or_update_weigth(
              from_vertex, to_vertex, weight, graph, *partitioner));
        }
      }
      try {
        partitioner->assign_partition(involved_vertices, config.N_PARTITIONS);
        statistics.add_edges(involved_edges);
      } catch (const std::logic_error &e) {
        cout << e.what() << endl;
        return 0;
      }
      std::unordered_set<uint32_t> partitions_involved(config.N_PARTITIONS);
      for (const auto &edge : involved_edges) {
        if (std::get<1>(edge) == Utils::INVALID)
          continue;
        auto from =
            Utils::get_id(boost::source(std::get<0>(edge), graph), graph);
        auto to = Utils::get_id(boost::target(std::get<0>(edge), graph), graph);
        auto fr_p = partitioner->m_partitioning[from];
        auto to_p = partitioner->m_partitioning[to];
        partitions_involved.insert(fr_p);
        partitions_involved.insert(to_p);
      }

      if (partitions_involved.size() == 1) {
        ++same_partition_tx_access;
      } else {
        ++cross_partition_tx_access;
      }
      for (const auto &p : partitions_involved) {
        ++txs_per_partition[p];
      }

      for (const auto vtx : delete_vertices) {
        partitioner->remove_vertex(vtx);
      }
    }
    statistics.log(new_timestamp);
  }
  partitioner->terminate();
  return 0;
}