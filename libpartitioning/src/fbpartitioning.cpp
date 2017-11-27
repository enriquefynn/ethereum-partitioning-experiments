#include <iostream>

#include <fbpartitioning.h>

std::vector<uint32_t>
FB_partitioner::get_neighbors(const std::vector<int32_t> &partitioning,
                              const Graph &g) {

  out_edge_it edg_it, edg_it_end;
  Edge ed;
  std::vector<uint32_t> partition_to_go(boost::num_vertices(g));
  // For every vertex, calcuate neighbors other partitions
  for (auto vertex = boost::vertices(g); vertex.first != vertex.second;
       ++vertex.first) {
    std::vector<uint32_t> neighboors_in_partition(N_PARTITIONS);
    // std::cout << boost::out_degree(*vertex.first, g) << std::endl;

    for (tie(edg_it, edg_it_end) = boost::out_edges(*vertex.first, g);
         edg_it != edg_it_end; ++edg_it) {
      ed = *edg_it;

      ++neighboors_in_partition[partitioning[boost::target(ed, g)]];
    }
    auto max_p = neighboors_in_partition[0];
    for (int i = 1; i < N_PARTITIONS; ++i)
      if (neighboors_in_partition[i] > max_p)
        max_p = neighboors_in_partition[i];

    // for (const auto &it : neighboors_in_partition)
    //   std::cout << it << ' ';
    // std::cout << std::endl;
  }

  return partition_to_go;
}