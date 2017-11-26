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

#define USE_METIS true 
#define N_NODES 27251679
#define METIS_SEED 1
#define N_PARTITIONS 2

#define TIME_GAP_LOG 60 * 60 * 4                 // 4 hours
#define TIME_REPARTITION 60 * 60 * 24 * 15       // 15 days
#define TIME_REPARTITION_WINDOW 60 * 60 * 24 * 2 // 2 days
#define CROSS_PARTITION_THRESHOLD 0.3 // Threshold for when trigger repartition

class Config {
  const std::string get_partitioning_mode();

public:
  enum { DEFAULT_METIS, HMETIS };
  enum { PERIODIC_PARTITIONING, DYNAMIC_PARTITIONING };
  uint8_t METIS_MODE;
  uint8_t PARTITIONING_MODE;
  Config(const std::string &input_config);
  const std::string get_METIS_name();

  Config() = default;
  Config(Config const &) = delete;
  Config(Config &&) = default;
};