#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iomanip>

#include <metis.h>

#include <HMETIS_methods.h>
#include <partitioner.h>

#include <utils.h>

HMETIS_partitioner::HMETIS_partitioner(const Graph &graph)
    : Partitioner(METIS_SEED, graph) {
  assert(m_seed > 0);
  METIS_OPTIONS[0] = 1; // Default parameters

  METIS_OPTIONS[1] = 1;      // Nruns
  METIS_OPTIONS[2] = 1;      // Ctype
  METIS_OPTIONS[3] = 1;      // Rtype
  METIS_OPTIONS[4] = 1;      // Scheme v-cycle refinement
  METIS_OPTIONS[5] = 0;      // Scheme reconstruct
  METIS_OPTIONS[6] = 0;      // Preassign
  METIS_OPTIONS[7] = m_seed; // Seed
  // METIS_OPTIONS[8] = 31;     // Debug
  METIS_OPTIONS[8] = 0; // Debug
}

uint32_t HMETIS_partitioner::partition(int nparts) {
  std::string hmetis_args = "-dbglvl=0 "
                            "-seed=" +
                            std::to_string(m_seed) +
                            " "
                            "-ufactor=" +
                            std::to_string(METIS_UBFACTOR) + " ";

  // out_edge_it edg_it, edg_it_end;
  // Edge ed;
  // // void partition_METIS(Graph &g, int nparts) {
  // std::pair<vertex_it, vertex_it> vertex;

  int nvtxs = boost::num_vertices(m_graph);
  int nhedges = m_hGraph.size();
  // if (!nvtxs)
  //   assert(false);
  // int *eptr = (int *)malloc((nhedges + 1) * sizeof(int));
  // int *eind = (int *)malloc((m_eind_size) * sizeof(int));
  // int *hewgts = (int *)malloc((nhedges) * sizeof(int));
  // int *part = (int *)malloc(nvtxs * sizeof(int));

  // int edge_idx = 0;
  // int edge_cut;
  // eptr[0] = 0;
  // int32_t v_idx = 1;

  std::ofstream hmetis_ex("/tmp/hmetis_file.hgr");
  hmetis_ex << nhedges << ' ' << nvtxs << std::endl;
  for (const auto &hedge : m_hGraph) {
    // hewgts[v_idx - 1] = hedge.second;
    for (const auto &vtx : hedge.first) {
      hmetis_ex << vtx + 1 << ' ';
      // eind[edge_idx] = vtx;
      // ++edge_idx;
    }
    hmetis_ex << std::endl;
    // eptr[v_idx] = edge_idx;
    // ++v_idx;
  }
  std::string call_command = "hmetis " + hmetis_args + "/tmp/hmetis_file.hgr ";
  call_command += std::to_string(nparts);
  call_command += " > /dev/null";

  std::cout << "Elapsed time to partition: "
            << Utils::measure_time(system, (call_command.c_str())) << std::endl;

  std::ifstream hmetis_res("/tmp/hmetis_file.hgr.part." +
                           std::to_string(nparts));

  // std::cout << "EITA" << std::endl;
  // std::cout << "nvtxs: " << nvtxs << std::endl;
  // std::cout << "Hedges: " << nhedges << std::endl;
  // std::cout << "m_eind_size: " << m_eind_size << std::endl;
  // std::cout << "vidx: " << v_idx << std::endl;
  // std::cout << "edge_idx: " << edge_idx << std::endl;

  // clock_t begin = clock();

  // std::cout << "eptr: ";
  // for (int i = 0; i < nhedges + 1; ++i)
  //   std::cout << eptr[i] << ' ';
  // std::cout << std::endl;
  // std::cout << "eind: ";
  // for (int i = 0; i < m_eind_size; ++i)
  //   std::cout << eind[i] << ' ';
  // std::cout << std::endl;

  // std::cout << "hewgts: ";
  // for (int i = 0; i < nhedges; ++i)
  //   std::cout << hewgts[i] << ' ';
  // std::cout << std::endl;

  // nhedges = 1;
  // nvtxs = 1000;
  // int eptra[2] = {0, 1000};
  // int einda[50];
  // for (int i = 0; i < 1000; ++i)
  //   einda[i] = i;

  // std::cout << "------" << std::endl;

  // HMETIS_PartRecursive(nvtxs,          // The number of vertices in the
  // graph.
  //                 nhedges,        // The number of hedges in the graph.
  //                 NULL,           // The weights of the vertices
  //                 eptr,          // The adjacency structure of the graph
  //                 eind,          // The adjacency structure of the graph
  //                 NULL,           // The weights of the edges
  //                 nparts,         // The number of parts to partition the
  //                 graph METIS_UBFACTOR, // UB factor METIS_OPTIONS,  // METIS
  //                 options part,           // Partitioning &edge_cut       //
  //                 Whatever
  // );

  // clock_t end = clock();
  // std::cout << "Elapsed time: " << double(end - begin) / CLOCKS_PER_SEC
  //           << std::endl;

  auto old_partitioning = std::move(m_partitioning);
  assert(m_partitioning.size() == 0);
  m_partitioning = std::vector<int32_t>(nvtxs);
  int vtx_p;
  int vtx_id = 0;
  while (hmetis_res >> vtx_p) {
    m_partitioning[vtx_id++] = vtx_p;
  }
  assert(vtx_id == nvtxs);
  // m_partitioning = std::vector<int>(part, part + nvtxs);

  // free(eptr);
  // free(eind);
  // free(hewgts);
  // free(part);
  return calculate_movements_repartition(old_partitioning, nparts);
}

bool HMETIS_partitioner::trigger_partitioning(
    uint32_t new_timestamp, uint32_t cross_edge_access,
    uint32_t same_partition_edge_access) {
  if (PARTITIONING_MODE == DYNAMIC_PARTITIONING) {
    cross_partition_calls += static_cast<float>(cross_edge_access);
    total_calls +=
        static_cast<float>(cross_edge_access + same_partition_edge_access);
    if (new_timestamp - timestamp_last_check > TIME_REPARTITION_WINDOW) {
      if (new_timestamp - timestamp_last_repartition > TIME_REPARTITION) {
        if ((cross_partition_calls / total_calls) > CROSS_PARTITION_THRESHOLD) {
          timestamp_last_repartition = new_timestamp;
          return true;
        }
      }
      cross_partition_calls = 0;
      total_calls = 0;
      timestamp_last_check = new_timestamp;
    }
    return false;
  } else if (PARTITIONING_MODE == PERIODIC_PARTITIONING) {
    if (new_timestamp - timestamp_last_repartition > TIME_REPARTITION) {
      timestamp_last_repartition = new_timestamp;
      return true;
    }
    return false;
  } else {
    assert(false);
  }
}

void HMETIS_partitioner::assign_partition(const std::set<uint32_t> &vertex_list,
                                          int32_t nparts) {
  Partitioner::assign_partition(vertex_list, nparts);

  // Build graph for HMETIS
  auto hedge = m_hGraph.find(vertex_list);
  if (hedge == m_hGraph.end()) {
    m_eind_size += vertex_list.size();
    m_hGraph[vertex_list] = 0;
  }
  ++m_hGraph[vertex_list];
}

std::string HMETIS_partitioner::get_name() {

  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << CROSS_PARTITION_THRESHOLD;
  std::string threshold = stream.str();

  std::string METIS_mode =
      "HMETIS_" + ((PARTITIONING_MODE == PERIODIC_PARTITIONING)
                       ? "PERIODIC_"
                       : "DYNAMIC_" + threshold + "_WINDOW_" +
                             std::to_string(TIME_REPARTITION_WINDOW) + "_");

  return METIS_mode + "repart_" + std::to_string(TIME_REPARTITION) + "_seed_" +
         std::to_string(METIS_SEED);
}