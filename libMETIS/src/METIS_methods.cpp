#include <cassert>
#include <metis.h>

#include <METIS_methods.h>
#include <config.h>
#include <partitioner.h>

#define ASSERT(left, operator, right)                                                              \
  {                                                                                                \
    if (!((left) operator(right))) {                                                               \
      std::cerr                                                                                    \
          << "ASSERT FAILED: " << #left                                                            \
          << #                                                                                     \
             operator<< #right << " @ " << __FILE__ << " (" << __LINE__ << ")"                     \
                                                                           "."                     \
                                                                           " " << #left << "=" <<( \
                                                                               left)               \
          << "; " << #right << "=" << (right) << std::endl;                                        \
    }                                                                                              \
  }

METIS::METIS(idx_t seed, const Graph &graph, const Config &config)
    : Partitioner(seed, graph, config) {
  assert(seed > 0);
  METIS_SetDefaultOptions(METIS_OPTIONS);
  METIS_OPTIONS[METIS_OPTION_OBJTYPE] = METIS_OBJTYPE_VOL;
  METIS_OPTIONS[METIS_OPTION_SEED] = seed;
  METIS_OPTIONS[METIS_OPTION_UFACTOR] = 300;
  // METIS_OPTIONS[METIS_OPTION_DBGLVL] = 2;
}

std::vector<idx_t> METIS::partition(idx_t nparts) {
  out_edge_it edg_it, edg_it_end;
  Edge ed;
  // void partition_METIS(Graph &g, idx_t nparts) {
  std::pair<vertex_it, vertex_it> vertex;

  idx_t nvtxs = boost::num_vertices(graph);
  if (!nvtxs)
    return std::vector<idx_t>();
  idx_t ncon = 1;
  idx_t *xadj = (idx_t *)malloc((nvtxs + 1) * sizeof(idx_t));
  idx_t *adjncy = (idx_t *)malloc(2 * boost::num_edges(graph) * sizeof(idx_t));
  idx_t *adjwgt = (idx_t *)malloc(2 * boost::num_edges(graph) * sizeof(idx_t));
  idx_t *part = (idx_t *)malloc(nvtxs * sizeof(idx_t));

  uint32_t weight, edge_idx = 0;
  xadj[0] = 0;
  int32_t v_idx = 1;
  for (vertex = boost::vertices(graph); vertex.first != vertex.second;
       ++vertex.first, ++v_idx) {
    for (tie(edg_it, edg_it_end) = boost::out_edges(*vertex.first, graph);
         edg_it != edg_it_end; ++edg_it) {
      ed = *edg_it;
      weight = boost::get(boost::edge_weight_t(), graph, ed);
      adjncy[edge_idx] = boost::target(ed, graph);
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

  std::vector<idx_t> partitioning = std::vector<idx_t>(part, part + nvtxs);
  free(xadj);
  free(adjncy);
  free(adjwgt);
  free(part);
  return partitioning;
}

bool METIS::trigger_partitioning(uint32_t new_timestamp,
                                 bool last_edge_cross_partition) {
  if (config.PARTITIONING_MODE == config.DYNAMIC_PARTITIONING) {
    ++total_calls;
    cross_partition_calls += last_edge_cross_partition;
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
  } else if (config.PARTITIONING_MODE == config.PERIODIC_PARTITIONING) {
    if (new_timestamp - timestamp_last_repartition > TIME_REPARTITION) {
      timestamp_last_repartition = new_timestamp;
      return true;
    }
    return false;
  } else
    assert(false);
}
