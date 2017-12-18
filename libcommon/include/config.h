#pragma once
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

typedef boost::property<boost::edge_weight_t, uint32_t> EdgeWeightProperty;
typedef boost::adjacency_list<boost::hash_setS, boost::vecS, boost::undirectedS,
                              boost::no_property, EdgeWeightProperty,
                              boost::hash_setS>
    Graph;

typedef boost::graph_traits<Graph> GraphTraits;
typedef boost::graph_traits<Graph>::vertex_iterator vertex_it;
typedef Graph::edge_descriptor Edge;
typedef GraphTraits::out_edge_iterator out_edge_it;

#define PERIODIC_PARTITIONING 0
#define DYNAMIC_PARTITIONING 1

#define TIME_GAP_LOG 60 * 60 * 4 // 4 hours

class Config {
public:
  enum {
    HASH_PARTITIONER,
    METIS_PARTITIONER,
    HMETIS_PARTITIONER,
    FACEBOOK_PARTITIONER,
    FILE_PARTITIONER
  };
  std::string FILE_PATH = "";
  uint32_t N_PARTITIONS = 2;
  uint8_t PARTITIONING_MODE = HASH_PARTITIONER;
  Config(const std::string &config_file);

  Config() = delete;
  Config(Config const &) = delete;
  Config(Config &&) = default;
  
};