#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include <metis.h>

#include <METIS_methods.h>
#include <config.h>
#include <fbpartitioning.h>
#include <hash_partitioning.h>
#include <utils.h>

#define USE_PARTITIONER METIS_PARTITIONER

using namespace std;

void add_edge_or_update_weigth(int from, int to, int weight, Graph &g) {
  boost::add_edge(from, to, 0, g);
  std::pair<Edge, bool> ed = boost::edge(from, to, g);
  uint32_t prev_weight = boost::get(boost::edge_weight_t(), g, ed.first);
  boost::put(boost::edge_weight_t(), g, ed.first, prev_weight + weight);
}

int main(int argc, char **argv) {
  Graph g;
  Partitioner *partitioner;
#if USE_PARTITIONER == METIS_PARTITIONER
cout << "Using METIS partitioner" << endl;
  METIS METIS_Graph(g);
  partitioner = &METIS_Graph;
#elif USE_PARTITIONER == NO_PARTITIONER
  Hash_partitioner hash_partitioner(g);
  partitioner = &hash_partitioner;
#elif USE_PARTITIONER == FACEBOOK_PARTITIONER
  FB_partitioner fb_partitioner(g);
  partitioner = &hash_partitioner;
#else
  assert(false)
#endif

  ifstream calls_file(argv[1]);
  ofstream stats_file("/tmp/edge_cut_evolution_partitions_" +
                      to_string(N_PARTITIONS) + "_period_" +
                      to_string(TIME_GAP_LOG) + "_" + partitioner->get_name() +
                      +".txt");

  uint32_t lineN, timestamp_log, new_timestamp;
  uint32_t from_vertex, to_vertex, weight;

  unordered_map<uint32_t, int32_t> vertex_partition;
  vector<idx_t> partitioning;
  uint32_t total_edge_access, cross_edge_access;
  bool last_edge_cross_partition = false;
  total_edge_access = cross_edge_access = 0;

  lineN = timestamp_log = 0;
  vector<string> tokens;
  string str;

  while (getline(calls_file, str)) {
    lineN += 1;
    tokens.clear();
    istringstream iss(str);
    copy(istream_iterator<string>(iss), istream_iterator<string>(),
         back_inserter(tokens));

    switch (tokens[0][0]) {
    // Genesis
    case 'G':
      to_vertex = stoi(tokens[1]);
      add_edge_or_update_weigth(0, to_vertex, 1, g);
      partitioner->assign_partition(partitioning, 0, to_vertex, N_PARTITIONS);
      continue;

    case 'B':
      new_timestamp = stoi(tokens[2]);
      if (!timestamp_log)
        timestamp_log = new_timestamp;
      if (partitioner->trigger_partitioning(new_timestamp,
                                            last_edge_cross_partition)) {
        // Partition the graph with METIS
        vector<idx_t> old_partitioning(partitioning);
        partitioning = partitioner->partition(N_PARTITIONS);
        uint32_t movements_to_repartition =
            partitioner->calculate_movements_repartition(
                old_partitioning, partitioning, N_PARTITIONS);

        // Calculate edge-cut / balance
        uint32_t edges_cut;
        vector<uint32_t> balance;
        tie(edges_cut, balance) =
            partitioner->calculate_edge_cut(g, partitioning);
        // LOG: REPARTITION Timestamp nVertices nMovements nEdges nEdgesCut
        // vVerticesEachPartition
        stats_file << "REPARTITION " << new_timestamp << ' '
                   << boost::num_vertices(g) << ' ' << boost::num_edges(g)
                   << ' ' << movements_to_repartition << ' ' << edges_cut
                   << ' ';
        for (int i = 0; i < N_PARTITIONS; ++i)
          stats_file << balance[i] << ' ';
        stats_file << endl;
      }
      assert(new_timestamp >= timestamp_log);
      if (new_timestamp - timestamp_log > TIME_GAP_LOG) {
        assert(total_edge_access >= cross_edge_access);
        stats_file << "POINT " << cross_edge_access << ' '
                   << (total_edge_access - cross_edge_access) << ' '
                   << new_timestamp << endl;

        total_edge_access = cross_edge_access = 0;
        timestamp_log = new_timestamp;
      }
      continue;

    case 'T':
      int author = stoi(tokens[1]);
      // tokens[1] : from
      int i = 3, type;
      while (i < tokens.size()) {
        type = stoi(tokens[i]);
        ++i;
        int numCalls = stoi(tokens[i]);

        ++i;
        for (int j = 0; j < numCalls; ++j) {
          if (tokens[i] == "1") {
            ++i;
            from_vertex = author;
            to_vertex = stoi(tokens[i]);
          } else {
            from_vertex = stoi(tokens[++i]);
            to_vertex = stoi(tokens[++i]);
          }
          if (type <= 2) {
            ++i;
            // value
          }
          ++i;
          // repetition
          weight = stoi(tokens[i]);

          // ADD edge
          add_edge_or_update_weigth(from_vertex, to_vertex, weight, g);
          partitioner->assign_partition(partitioning, from_vertex, to_vertex,
                                        N_PARTITIONS);
          ++total_edge_access;
          if (partitioning[from_vertex] != partitioning[to_vertex]) {
            ++cross_edge_access;
            last_edge_cross_partition = true;
          } else
            last_edge_cross_partition = false;

          ++i;
        }
      }
      continue;
    }
  }
  // FBPartitioning fbpartitioning;
  // fbpartitioning.get_neighbors(partitioning, g);
}

//     new_timestamp = stoi(tokens[tokens.size() - 2]);

// #if USE_METIS
//     if (METIS_Graph.trigger_partitioning(new_timestamp,
//                                          last_edge_cross_partition)) {
//       // Partition the graph with METIS
//       vector<idx_t> old_partitioning(partitioning);
//       partitioning = METIS_Graph.partition_METIS(g, N_PARTITIONS);
//       uint32_t movements_to_repartition =
//           METIS_Graph.calculate_movements_repartition(
//               old_partitioning, partitioning, N_PARTITIONS);

//       // Calculate edge-cut / balance
//       uint32_t edges_cut;
//       vector<uint32_t> balance;
//       tie(edges_cut, balance) = METIS_Graph.calculate_edge_cut(g,
//       partitioning);
//       // LOG: REPARTITION Timestamp nVertices nMovements nEdges nEdgesCut
//       // vVerticesEachPartition
//       stats_file << "REPARTITION " << new_timestamp << ' '
//                  << boost::num_vertices(g) << ' ' << boost::num_edges(g) << '
//                  '
//                  << movements_to_repartition << ' ' << edges_cut << ' ';
//       for (int i = 0; i < N_PARTITIONS; ++i)
//         stats_file << balance[i] << ' ';
//       stats_file << endl;
//     }
// #endif

//     from_vertex = stoi(tokens[2]);
//     to_vertex = stoi(tokens[3]);

// #if USE_METIS
//     METIS_Graph.assign_partition_same(partitioning, from_vertex, to_vertex,
//                                       N_PARTITIONS);
// #else
//     Utils::assign_hash_parititon(partitioning, from_vertex, to_vertex,
//                                  N_PARTITIONS);
// #endif
//     ++total_edge_access;
//     if (partitioning[from_vertex] != partitioning[to_vertex]) {
//       ++cross_edge_access;
//       last_edge_cross_partition = true;
//     } else
//       last_edge_cross_partition = false;

//     boost::add_edge(from_vertex, to_vertex, 0, g);
//     std::pair<Edge, bool> ed = boost::edge(from_vertex, to_vertex, g);
//     uint32_t weight = boost::get(boost::edge_weight_t(), g, ed.first);
//     boost::put(boost::edge_weight_t(), g, ed.first, weight + 1);

//     assert(new_timestamp >= timestamp_log);
//     if (new_timestamp - timestamp_log > TIME_GAP_LOG) {
//       assert(total_edge_access >= cross_edge_access);
//       stats_file << "POINT " << cross_edge_access << ' '
//                  << (total_edge_access - cross_edge_access) << ' '
//                  << new_timestamp << endl;

//       total_edge_access = cross_edge_access = 0;
//       timestamp_log = new_timestamp;
//     }