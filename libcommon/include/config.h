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
#define NO_PARTITIONER 0
#define METIS_PARTITIONER 1
#define FACEBOOK_PARTITIONER 2

#define PERIODIC_PARTITIONING 0
#define DYNAMIC_PARTITIONING 1 

#define N_PARTITIONS 2
#define TIME_GAP_LOG 60 * 60 * 4 // 4 hours