#include <cassert>
#include <iomanip>
#include <metis.h>

#include <METIS_methods.h>
#include <partitioner.h>

METIS_partitioner::METIS_partitioner(const Graph &graph)
    : Partitioner(METIS_SEED, graph) {
  assert(m_seed > 0);
  METIS_SetDefaultOptions(METIS_OPTIONS);
  METIS_OPTIONS[METIS_OPTION_OBJTYPE] = METIS_OBJTYPE_VOL;
  METIS_OPTIONS[METIS_OPTION_SEED] = m_seed;
  METIS_OPTIONS[METIS_OPTION_UFACTOR] = 300;
  // METIS_OPTIONS[METIS_OPTION_DBGLVL] = 2;
}

uint32_t METIS_partitioner::partition(idx_t nparts) {
  out_edge_it edg_it, edg_it_end;
  Edge ed;
  // void partition_METIS(Graph &g, idx_t nparts) {
  std::pair<vertex_it, vertex_it> vertex;

  idx_t nvtxs = boost::num_vertices(m_graph);
  if (!nvtxs)
    assert(false);
  idx_t ncon = 1;
  idx_t *xadj = (idx_t *)malloc((nvtxs + 1) * sizeof(idx_t));
  idx_t *adjncy =
      (idx_t *)malloc(2 * boost::num_edges(m_graph) * sizeof(idx_t));
  idx_t *adjwgt =
      (idx_t *)malloc(2 * boost::num_edges(m_graph) * sizeof(idx_t));
  idx_t *part = (idx_t *)malloc(nvtxs * sizeof(idx_t));

  uint32_t weight, edge_idx = 0;
  xadj[0] = 0;
  int32_t v_idx = 1;
  for (vertex = boost::vertices(m_graph); vertex.first != vertex.second;
       ++vertex.first, ++v_idx) {
    for (tie(edg_it, edg_it_end) = boost::out_edges(*vertex.first, m_graph);
         edg_it != edg_it_end; ++edg_it) {
      ed = *edg_it;
      weight = boost::get(boost::edge_weight_t(), m_graph, ed);
      adjncy[edge_idx] = boost::target(ed, m_graph);
      adjwgt[edge_idx] = weight;
      ++edge_idx;
    }
    xadj[v_idx] = edge_idx;
  }

  idx_t objval;
  int return_METIS = METIS_PartGraphKway(
      &nvtxs,  // The number of vertices in the graph.
      &ncon,   // The number of balancing constraints
      xadj,    // The adjacency structure of the graph
      adjncy,  // The adjacency structure of the graph
      NULL,    // The weights of the vertices
      NULL,    // Size of vertices for computing the total communication volume
      adjwgt,  // The weights of the edges
      &nparts, // The number of parts to partition the graph
      NULL, // nparts×ncon that specifies the desired weight for each partition
      NULL, // array of size ncon that specifies the allowed load imbalance
            // tolerance for each constraint
      METIS_OPTIONS, // METIS options
      &objval, part);
  assert(return_METIS == METIS_OK);

  auto old_partitioning = std::move(m_partitioning);
  m_partitioning = std::vector<idx_t>(part, part + nvtxs);

  free(xadj);
  free(adjncy);
  free(adjwgt);
  free(part);
  return calculate_movements_repartition(old_partitioning, nparts);
}

bool METIS_partitioner::trigger_partitioning(
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

std::string METIS_partitioner::get_name() {

  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << CROSS_PARTITION_THRESHOLD;
  std::string threshold = stream.str();

  std::string METIS_mode =
      "METIS_" + ((PARTITIONING_MODE == PERIODIC_PARTITIONING)
                      ? "PERIODIC_"
                      : "DYNAMIC_" + threshold + "_WINDOW_" +
                            std::to_string(TIME_REPARTITION_WINDOW) + "_");

  return METIS_mode + "repart_" + std::to_string(TIME_REPARTITION) + "_seed_" +
         std::to_string(METIS_SEED);
}