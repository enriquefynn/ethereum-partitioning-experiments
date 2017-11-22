
#include <METIS_methods.h>
#include <config.h>
#include <cassert>
#include <metis.h>

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

METIS::METIS(idx_t seed, const Config &_config) : config(_config) {
  assert(seed > 0);
  METIS_SetDefaultOptions(METIS_OPTIONS);
  METIS_OPTIONS[METIS_OPTION_OBJTYPE] = METIS_OBJTYPE_VOL;
  METIS_OPTIONS[METIS_OPTION_SEED] = seed;
  METIS_OPTIONS[METIS_OPTION_UFACTOR] = 300;
  // METIS_OPTIONS[METIS_OPTION_DBGLVL] = 2;
}

std::vector<idx_t> METIS::partition_METIS(const Graph &g, idx_t nparts) {
  typename GraphTraits::edge_descriptor ed;
  typename GraphTraits::out_edge_iterator edg_it, edg_it_end;
  // void partition_METIS(Graph &g, idx_t nparts) {
  std::pair<vertex_iter, vertex_iter> vertex;

  idx_t nvtxs = boost::num_vertices(g);
  if (!nvtxs)
    return std::vector<idx_t>();
  idx_t ncon = 1;
  idx_t *xadj = (idx_t *)malloc((nvtxs + 1) * sizeof(idx_t));
  idx_t *adjncy = (idx_t *)malloc(2 * boost::num_edges(g) * sizeof(idx_t));
  idx_t *adjwgt = (idx_t *)malloc(2 * boost::num_edges(g) * sizeof(idx_t));
  idx_t *part = (idx_t *)malloc(nvtxs * sizeof(idx_t));

  uint32_t weight, edge_idx = 0;
  xadj[0] = 0;
  int32_t v_idx = 1;
  for (vertex = boost::vertices(g); vertex.first != vertex.second;
       ++vertex.first, ++v_idx) {
    for (tie(edg_it, edg_it_end) = boost::out_edges(*vertex.first, g);
         edg_it != edg_it_end; ++edg_it) {
      ed = *edg_it;
      weight = boost::get(boost::edge_weight_t(), g, ed);
      adjncy[edge_idx] = ed.m_target;
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

uint32_t METIS::calculate_movements_repartition(
    const std::vector<idx_t> &old_partitioning,
    const std::vector<idx_t> &new_partitioning, idx_t nparts) {
  ASSERT(old_partitioning.size(), ==, new_partitioning.size());

  uint32_t moves = 0;
  const size_t new_partitioning_size = new_partitioning.size();
  const size_t old_partitioning_size = old_partitioning.size();

  for (size_t i = 0; i < old_partitioning_size; ++i)
    if (old_partitioning[i] != new_partitioning[i])
      ++moves;
  for (size_t i = old_partitioning_size; i < new_partitioning_size; ++i)
    if (static_cast<idx_t>(i % nparts) != new_partitioning[i])
      ++moves;
  return moves;
}

void METIS::assign_partition_hash(std::vector<idx_t> &partitioning,
                                  uint32_t vertex, idx_t nparts) {
  if (vertex == partitioning.size()) {
    partitioning.push_back(vertex % nparts);
  }
}

void METIS::assign_partition_same(std::vector<idx_t> &partitioning,
                                  uint32_t from_vertex, uint32_t to_vertex,
                                  idx_t nparts) {
  // NO partition for from_vertex
  if (from_vertex == partitioning.size()) {
    // Self-reference
    if (to_vertex == from_vertex) {
      partitioning.push_back(from_vertex % nparts);
    }
    // No partition for both vertices
    else if (to_vertex >= partitioning.size()) {
      partitioning.push_back(from_vertex % nparts);
      partitioning.push_back(to_vertex % nparts);
    }
    // Partitioning to to_vertex exist
    else {
      partitioning.push_back(partitioning[to_vertex]);
    }
  }
  // to_vertex has no partition
  else if (to_vertex == partitioning.size()) {
    if (from_vertex == to_vertex) {
      partitioning.push_back(from_vertex % nparts);
    }
    // No partition for both vertices
    else if (from_vertex > partitioning.size()) {
      partitioning.push_back(from_vertex % nparts);
      partitioning.push_back(to_vertex % nparts);
    }
    // Partitioning to to_vertex exist
    else {
      partitioning.push_back(partitioning[from_vertex]);
    }
  }
}

std::tuple<uint32_t, std::vector<uint32_t>>
METIS::calculate_edge_cut(const Graph &g,
                          const std::vector<idx_t> &partitioning) {
  typename GraphTraits::edge_iterator ei, ei_end;
  uint32_t edges_cut = 0;

  std::vector<uint32_t> balances(N_PARTITIONS);
  for (auto vertex = boost::vertices(g); vertex.first != vertex.second;
       ++vertex.first)
    ++balances[partitioning[*vertex.first]];

  for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    if (partitioning[boost::source(*ei, g)] !=
        partitioning[boost::target(*ei, g)])
      ++edges_cut;

  return make_tuple(edges_cut, balances);
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
