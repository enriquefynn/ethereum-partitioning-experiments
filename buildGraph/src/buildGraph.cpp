#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <metis.h>

#include <HMETIS_methods.h>
#include <METIS_methods.h>
#include <config.h>
#include <fbpartitioning.h>
#include <file_partitioning.h>
#include <hash_partitioning.h>
#include <utils.h>

using namespace std;

int main(int argc, char **argv) {
  Graph g;
  Config config = Config(argv[2]);

  Partitioner *partitioner;
  switch (config.PARTITIONING_MODE) {
  case config.HASH_PARTITIONER:
    cout << "Using Hash partitioner" << endl;
    partitioner = new Hash_partitioner(g, config);
    break;
  case config.METIS_PARTITIONER:
    cout << "Using METIS partitioner" << endl;
    partitioner = new METIS_partitioner(g, config);
    break;
  case config.HMETIS_PARTITIONER:
    cout << "Using Hyper-METIS partitioner" << endl;
    partitioner = new HMETIS_partitioner(g, config);
    break;
  case config.FACEBOOK_PARTITIONER:
    cout << "Using Facebook partitioner" << endl;
    partitioner = new FB_partitioner(g, config);
    break;
  case config.FILE_PARTITIONER:
    cout << "Using File partitioner" << endl;
    partitioner = new File_partitioner(g, config);
    break;

  default:
    assert(false);
    break;
  }

  ifstream calls_file(argv[1]);
  ofstream stats_file("/tmp/edge_cut_evolution_partitions_" +
                      to_string(N_PARTITIONS) + "_period_" +
                      to_string(TIME_GAP_LOG) + "_" + partitioner->get_name() +
                      +".txt");

  uint32_t lineN, timestamp_log, new_timestamp;
  uint32_t from_vertex, to_vertex, weight;

  uint32_t block_number, has_from = 2;
  uint32_t tx_type, tx_author, tx_type_n, tx_types_size;
  bool has_value, tx_failed;
  char header;
  string tx_value;

  uint32_t cross_edge_access, same_partition_edge_access;
  same_partition_edge_access = cross_edge_access = 0;

  lineN = timestamp_log = 0;

  set<uint32_t> involved_vertices;
  vector<pair<uint32_t, uint32_t>> involved_edges;
  vector<uint32_t> delete_vertices;

  // Genesis processing
  uint32_t n_transactions;

  calls_file >> header >> n_transactions;
  involved_vertices.insert(0);
  for (int tx = 0; tx < n_transactions; ++tx) {
    calls_file >> to_vertex >> tx_value;
    involved_vertices.insert(to_vertex);
    Utils::add_edge_or_update_weigth(0, to_vertex, 1, g);
  }
  partitioner->assign_partition(involved_vertices, N_PARTITIONS);

  while (calls_file >> header >> block_number >> new_timestamp >>
         n_transactions) {
    assert(header == 'B');

    if (!timestamp_log)
      timestamp_log = new_timestamp;

    if (partitioner->trigger_partitioning(new_timestamp, cross_edge_access,
                                          same_partition_edge_access)) {
      // Partition the graph
      auto before = std::chrono::high_resolution_clock::now();
      uint32_t movements_to_repartition = partitioner->partition(N_PARTITIONS);
      auto time_to_partition =
          std::chrono::duration_cast<std::chrono::milliseconds>(
              (std::chrono::high_resolution_clock::now() - before))
              .count();
      cout << "Partitioning took: " << time_to_partition << "ms\n";
      // Calculate edge-cut / balance
      uint32_t edges_cut;

      vector<uint32_t> balance;
      tie(edges_cut, balance) = partitioner->calculate_edge_cut(g);
      // LOG: REPARTITION Timestamp nVertices nMovements nEdges nEdgesCut
      // vVerticesEachPartition
      stats_file << "REPARTITION " << new_timestamp << ' '
                 << boost::num_vertices(g) << ' ' << boost::num_edges(g) << ' '
                 << movements_to_repartition << ' ' << edges_cut << ' ';
      for (int i = 0; i < N_PARTITIONS; ++i)
        stats_file << balance[i] << ' ';
      stats_file << endl;
    }

    assert(new_timestamp >= timestamp_log);
    if (new_timestamp - timestamp_log > TIME_GAP_LOG) {
      stats_file << "POINT " << cross_edge_access << ' '
                 << same_partition_edge_access << ' ' << new_timestamp << ' ';
      for (int i = 0; i < N_PARTITIONS; ++i)
        stats_file << partitioner->m_balance[i] << ' ';
      stats_file << endl;
      same_partition_edge_access = cross_edge_access = 0;
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
          involved_vertices.insert(from_vertex);
          involved_vertices.insert(to_vertex);
          if (Utils::is_selfdestruct(tx_type) && !tx_failed) {
            // From: contract that was suicided
            // To: Funds moved there
            // Utils::remove_vertex(from_vertex, g, partitioner);
            delete_vertices.push_back(from_vertex);
            continue;
          }
          Utils::add_edge_or_update_weigth(from_vertex, to_vertex, weight, g);
          // cout << "Add " << from_vertex << " to " << to_vertex << endl;
          involved_edges.push_back({from_vertex, to_vertex});
        }
        partitioner->assign_partition(involved_vertices, N_PARTITIONS);
        for (const auto &edge : involved_edges) {
          if (!partitioner->same_partition(edge.first, edge.second)) {
            ++cross_edge_access;
          } else {
            ++same_partition_edge_access;
          }
        }
      }
    }
  }

  // while (getline(calls_file, str)) {
  //   lineN += 1;
  //   tokens.clear();
  //   istringstream iss(str);
  //   copy(istream_iterator<string>(iss), istream_iterator<string>(),
  //        back_inserter(tokens));

  //   assert(tokens[0][0] == 'B');

  //   switch (tokens[0][0]) {
  //   // Genesis
  //   case 'G': {
  //     processing_genesis = true;
  //     to_vertex = stoi(tokens[1]);
  //     involved_vertices.insert(0);
  //     involved_vertices.insert(to_vertex);
  //     Utils::add_edge_or_update_weigth(0, to_vertex, 1, g);
  //     break;
  //   }
  //   case 'B': {
  //     if (processing_genesis) {
  //       partitioner->assign_partition(involved_vertices, N_PARTITIONS);
  //       processing_genesis = false;
  //     }
  //     new_timestamp = stoi(tokens[2]);
  //     if (!timestamp_log)
  //       timestamp_log = new_timestamp;
  //     if (partitioner->trigger_partitioning(new_timestamp,
  //     cross_edge_access,
  //                                           same_partition_edge_access)) {
  //       // Partition the graph
  //       uint32_t movements_to_repartition =
  //           partitioner->partition(N_PARTITIONS);

  //       // Calculate edge-cut / balance
  //       uint32_t edges_cut;
  //       vector<uint32_t> balance;
  //       tie(edges_cut, balance) = partitioner->calculate_edge_cut(g);
  //       // LOG: REPARTITION Timestamp nVertices nMovements nEdges nEdgesCut
  //       // vVerticesEachPartition
  //       stats_file << "REPARTITION " << new_timestamp << ' '
  //                  << boost::num_vertices(g) << ' ' << boost::num_edges(g)
  //                  << ' ' << movements_to_repartition << ' ' << edges_cut
  //                  << ' ';
  //       for (int i = 0; i < N_PARTITIONS; ++i)
  //         stats_file << balance[i] << ' ';
  //       stats_file << endl;
  //     }
  //     assert(new_timestamp >= timestamp_log);
  //     if (new_timestamp - timestamp_log > TIME_GAP_LOG) {
  //       stats_file << "POINT " << cross_edge_access << ' '
  //                  << same_partition_edge_access << ' ' << new_timestamp <<
  //                  '
  //                  ';
  //       for (int i = 0; i < N_PARTITIONS; ++i)
  //         stats_file << partitioner->m_balance[i] << ' ';
  //       stats_file << endl;
  //       same_partition_edge_access = cross_edge_access = 0;
  //       timestamp_log = new_timestamp;
  //     }
  //     break;
  //   }

  //   case 'T': {
  //     involved_vertices.clear();
  //     vector<pair<uint32_t, uint32_t>> involved_edges;
  //     vector<uint32_t> delete_vertices;
  //     uint32_t author = stoi(tokens[1]);
  //     bool tx_failed = stoi(tokens[2]);
  //     involved_vertices.insert(author);
  //     // tokens[1] : from
  //     int i = 3, type;
  //     while (i < tokens.size()) {
  //       type = stoi(tokens[i]);
  //       ++i;
  //       int numCalls = stoi(tokens[i]);
  //       ++i;
  //       for (int j = 0; j < numCalls; ++j) {
  //         if (tokens[i] == "1") {
  //           ++i;
  //           from_vertex = author;
  //           to_vertex = stoi(tokens[i]);
  //         } else {
  //           from_vertex = stoi(tokens[++i]);
  //           to_vertex = stoi(tokens[++i]);
  //         }
  //         if (Utils::has_value(type)) {
  //           ++i;
  //           // value
  //         }
  //         ++i;
  //         // repetition
  //         weight = stoi(tokens[i]);

  //         // ADD edge
  //         if (Utils::is_selfdestruct(type) && !tx_failed) {
  //           // From: contract that was suicided
  //           // To: Funds moved there
  //           // Utils::remove_vertex(from_vertex, g, partitioner);
  //           involved_vertices.insert(to_vertex);
  //           involved_vertices.insert(from_vertex);
  //           delete_vertices.push_back(from_vertex);
  //         } else {
  //           Utils::add_edge_or_update_weigth(from_vertex, to_vertex,
  //           weight, g); involved_vertices.insert(from_vertex);
  //           involved_vertices.insert(to_vertex);
  //           involved_edges.push_back({from_vertex, to_vertex});
  //         }
  //         ++i;
  //       }
  //     }
  //     partitioner->assign_partition(involved_vertices, N_PARTITIONS);
  //     for (const auto &edge : involved_edges) {
  //       if (!partitioner->same_partition(edge.first, edge.second)) {
  //         ++cross_edge_access;
  //       } else {
  //         ++same_partition_edge_access;
  //       }
  //     }
  //     if (delete_vertices.size())
  //     // cout << "REMOVE: " << delete_vertices.size() << endl;
  //     for (const auto vtx : delete_vertices) {
  //       // cout << "DELETE: " << vtx << endl;
  //       if (partitioner->m_partitioning[vtx] == N_PARTITIONS)
  //         cout << "Timestamp: " << new_timestamp << endl << "vtx: " << vtx
  //         << endl;
  //       Utils::remove_vertex(vtx, g, partitioner);
  //       // cout << "vtx: " << vtx << endl;
  //       // cout << "BALANCE: " << partitioner->m_balance[0] << ' ' <<
  //       partitioner->m_balance[1] << endl;
  //     }
  //     break;
  //   }
  //   }
  // }
  return 0;
}