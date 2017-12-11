#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <unordered_map>
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

  int nvtxs = boost::num_vertices(m_graph);
  int nhedges = m_hGraph.size();

  std::ofstream hmetis_ex("/tmp/hmetis_file.hgr");
  hmetis_ex << nhedges << ' ' << nvtxs << std::endl;

  // Remove vertices stated to removal
  for (auto hedge_it = m_hGraph.begin(); hedge_it != m_hGraph.end();
       ++hedge_it) {
    std::vector<uint32_t> intersection;
    std::set_intersection(m_deleted_vertices.begin(), m_deleted_vertices.end(),
                          (*hedge_it).first.begin(), (*hedge_it).first.end(),
                          std::back_inserter(intersection));
    if (intersection.size() > 0) {
      auto new_hedge = std::set<uint32_t>((*hedge_it).first);
      for (const auto &remove_vtx : intersection) {
        new_hedge.erase(remove_vtx);
      }
      if (new_hedge.size() == 0) {
        m_hGraph.erase(hedge_it);
      } else {
        m_hGraph[new_hedge] = (*hedge_it).second;
      }
    }
  }
  m_deleted_vertices.clear();

  std::unordered_map<uint32_t, uint32_t> to_metis_vtx;
  uint32_t next_id = 1;
  auto get_metis_id = [&](uint32_t vtx) {
    if (to_metis_vtx.count(vtx) == 0) {
      to_metis_vtx[vtx] = next_id++;
    }
    return to_metis_vtx[vtx];
  };

  for (const auto &hedge : m_hGraph) {
    for (const auto &vtx : hedge.first) {
      hmetis_ex << get_metis_id(vtx) << ' ';
    }
    hmetis_ex << std::endl;
  }
  std::string call_command = "hmetis " + hmetis_args + "/tmp/hmetis_file.hgr ";
  call_command += std::to_string(nparts);
  call_command += " > /dev/null";

  std::cout << "Elapsed time to partition: "
            << Utils::measure_time(system, (call_command.c_str())) << std::endl;

  std::ifstream hmetis_res("/tmp/hmetis_file.hgr.part." +
                           std::to_string(nparts));

  auto old_partitioning = std::move(m_partitioning);
  assert(m_partitioning.size() == 0);
  // m_partitioning = std::vector<int32_t>(nvtxs);
  int vtx_p;
  int vtx_id = 0;
  while (hmetis_res >> vtx_p) {
    m_partitioning[vtx_id++] = vtx_p;
  }
  assert(vtx_id == nvtxs);

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

void HMETIS_partitioner::remove_vertex(uint32_t vtx) {
  Partitioner::remove_vertex(vtx);
  m_deleted_vertices.insert(vtx);
}