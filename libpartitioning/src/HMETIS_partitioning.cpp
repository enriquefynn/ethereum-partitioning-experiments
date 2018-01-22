#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <metis.h>
#include <unordered_map>

#include <HMETIS_partitioning.h>
#include <partitioner.h>

#include <utils.h>

HMETIS_partitioner::HMETIS_partitioner(Graph &graph, Config &config)
    : Partitioner(METIS_SEED, graph, config) {
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
                            std::to_string(m_seed) + " " + //" -ptype=kway " +
                            "-ufactor=" + std::to_string(METIS_UBFACTOR) + " ";

  int nvtxs = boost::num_vertices(m_graph);
  int nhedges = m_hGraph.size();

  // Remove vertices stated to removal
  // for (auto hedge_it = m_hGraph.begin(); hedge_it != m_hGraph.end();
  //      ++hedge_it) {
  //   std::vector<uint32_t> intersection;
  //   std::set_intersection(m_deleted_vertices.begin(),
  //   m_deleted_vertices.end(),
  //                         (*hedge_it).first.begin(), (*hedge_it).first.end(),
  //                         std::back_inserter(intersection));
  //   if (intersection.size() > 0) {
  //     auto new_hedge = std::set<uint32_t>((*hedge_it).first);
  //     for (const auto &remove_vtx : intersection) {
  //       new_hedge.erase(remove_vtx);
  //     }
  //     if (new_hedge.size() == 0) {
  //       m_hGraph.erase(hedge_it);
  //     } else {
  //       m_hGraph[new_hedge] = (*hedge_it).second;
  //     }
  //   }
  // }
  // m_deleted_vertices.clear();

  std::unordered_map<uint32_t, uint32_t> to_metis_vtx;
  std::unordered_map<uint32_t, uint32_t> from_metis_vtx;
  uint32_t next_id = 1;
  auto get_metis_id = [&](uint32_t vtx) {
    if (to_metis_vtx.count(vtx) == 0) {
      to_metis_vtx[vtx] = next_id;
      from_metis_vtx[next_id] = vtx;
      ++next_id;
    }
    return to_metis_vtx[vtx];
  };
  std::string hmetis_filename =
      "/tmp/hmetis_file.part_" + std::to_string(nparts) + ".hgr";

  std::ofstream hmetis_ex(hmetis_filename);
  hmetis_ex << nhedges << ' ' << nvtxs << std::endl;

  for (const auto &hedge : m_hGraph) {
    for (const auto &vtx : hedge.first) {
      hmetis_ex << get_metis_id(vtx) << ' ';
      // hmetis_ex << vtx + 1 << ' ';
    }
    hmetis_ex << std::endl;
  }
  std::string call_command = "hmetis " + hmetis_args + " " + hmetis_filename +
                             " " + std::to_string(nparts) + " > /dev/null";

  system(call_command.c_str());

  std::ifstream hmetis_res(hmetis_filename + ".part." + std::to_string(nparts));

  auto old_partitioning = std::move(m_partitioning);
  assert(m_partitioning.size() == 0);
  // m_partitioning = std::vector<int32_t>(nvtxs);
  int vtx_p;
  int vtx_id = 1;
  while (hmetis_res >> vtx_p) {
    m_partitioning[from_metis_vtx[vtx_id]] = vtx_p;
    ++vtx_id;
  }

  // assert(vtx_id == nvtxs);
  return calculate_movements_repartition(old_partitioning, nparts);
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
  
  if (m_config.SAVE_PARTITIONING) {
    for (auto const &vertex : vertex_list) {
      m_saved_partitioning[vertex] = m_partitioning[vertex];
    }
  }
}

std::string HMETIS_partitioner::get_name() {
  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << m_config.CROSS_PARTITION_THRESHOLD;
  std::string threshold = stream.str();

  std::string METIS_mode =
      "HMETIS_" + ((m_config.PARTITIONING_TYPE == Config::PERIODIC_PARTITIONING)
                       ? "PERIODIC_"
                       : "DYNAMIC_" + threshold + "_WINDOW_" +
                             std::to_string(m_config.TIME_REPARTITION_WINDOW) + "_");

  return METIS_mode + "repart_" + std::to_string(m_config.TIME_REPARTITION) + "_seed_" +
         std::to_string(METIS_SEED);
}

void HMETIS_partitioner::remove_vertex(uint32_t vtx) {
  Partitioner::remove_vertex(vtx);
  m_deleted_vertices.insert(vtx);
}