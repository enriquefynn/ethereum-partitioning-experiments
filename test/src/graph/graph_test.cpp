
#include <gtest/gtest.h>

#include <boost/graph/graph_utility.hpp>
#include <config.h>
#include <fstream>
#include <hash_partitioning.h>
#include <statistics.h>
#include <unordered_map>
#include <utils.h>

typedef std::unordered_map<uint32_t, uint32_t> map_type;

using namespace std;
using P = Utils::TUPLE_PROP;
class GraphTest : public ::testing::Test {
public:
  GraphTest() {}

protected:
  virtual void SetUp() {
    g = std::unique_ptr<Graph>(new Graph());
    config =
        std::unique_ptr<Config>(new Config("./test/src/graph/test_config.txt"));

    partitioning = std::unique_ptr<map_type>(new map_type());
    partitioner =
        std::unique_ptr<Partitioner>(new Hash_partitioner(*g, *config));
  }
  virtual void TearDown() {
    // do nothing
  }

protected:
  std::unique_ptr<Graph> g;
  std::unique_ptr<Config> config;
  std::unique_ptr<map_type> partitioning;
  std::unique_ptr<Partitioner> partitioner;
};

TEST_F(GraphTest, addEdgesTest) {
  Utils::add_edge_or_update_weigth(0, 1, 1, *g, *partitioner);
  Utils::add_edge_or_update_weigth(1, 22, 1, *g, *partitioner);
  Utils::add_edge_or_update_weigth(3, 4, 1, *g, *partitioner);
  Utils::add_edge_or_update_weigth(3, 10, 1, *g, *partitioner);
  set<int> vtxs = {0, 1, 22, 3, 4, 10};

  ASSERT_EQ(6, boost::num_vertices(*g));
  ASSERT_EQ(4, boost::num_edges(*g));
  for (auto vertex = boost::vertices(*g); vertex.first != vertex.second;
       ++vertex.first) {
    auto vertex_id = Utils::get_id(*vertex.first, *g);
    vtxs.erase(vertex_id);
  }
  ASSERT_EQ(vtxs.size(), 0);
}

TEST_F(GraphTest, noMultiEdge) {
  Utils::add_edge_or_update_weigth(1, 2, 1, *g, *partitioner);
  Utils::add_edge_or_update_weigth(1, 2, 1, *g, *partitioner);
  Utils::add_edge_or_update_weigth(1, 2, 1, *g, *partitioner);

  ASSERT_EQ(2, boost::num_vertices(*g));
  ASSERT_EQ(1, boost::num_edges(*g));
}

TEST_F(GraphTest, noLoop) {
  Utils::add_edge_or_update_weigth(1, 1, 1, *g, *partitioner);
  Utils::add_edge_or_update_weigth(2, 2, 1, *g, *partitioner);
  Utils::add_edge_or_update_weigth(2, 2, 1, *g, *partitioner);
  Utils::add_edge_or_update_weigth(3, 3, 1, *g, *partitioner);

  ASSERT_EQ(3, boost::num_vertices(*g));
  ASSERT_EQ(0, boost::num_edges(*g));
}

TEST_F(GraphTest, haveEdgeWeight) {
  Edge edge;
  auto added_edge =
      Utils::add_edge_or_update_weigth(1, 10, 10, *g, *partitioner);
  Utils::add_edge_or_update_weigth(1, 10, 1, *g, *partitioner);
  Utils::add_edge_or_update_weigth(1, 10, 1, *g, *partitioner);

  auto weights_map = get(boost::edge_weight, *g);
  auto weight = get(weights_map, std::get<P::EDGE>(added_edge));

  ASSERT_EQ(12, weight);
}

TEST_F(GraphTest, haveVertexWeight) {
  Edge edge;
  auto added_edge =
      Utils::add_edge_or_update_weigth(1, 10, 10, *g, *partitioner);
  Utils::add_edge_or_update_weigth(1, 10, 1, *g, *partitioner);
  Utils::add_edge_or_update_weigth(1, 10, 1, *g, *partitioner);

  ASSERT_EQ((*g)[boost::target(std::get<P::EDGE>(added_edge), *g)].m_vertex_weight,
            3);
  ASSERT_EQ((*g)[boost::source(std::get<P::EDGE>(added_edge), *g)].m_vertex_weight,
            3);
}

TEST_F(GraphTest, computesCorrectCC) {
  config->GRAPH_CC_PATH = "/tmp/cc.txt";
  auto stats = Statistics(*g, *partitioner, *config);
  partitioner->assign_partition({0, 1, 2, 3, 4, 5, 6}, 2);

  auto edge = Utils::add_edge_or_update_weigth(0, 1, 1, *g, *partitioner);
  stats.add_edges({edge});
  ASSERT_EQ(stats.m_cc_size.size(), 1);
  ASSERT_EQ(stats.m_cc_size[1], 2);
  
  edge = Utils::add_edge_or_update_weigth(1, 2, 1, *g, *partitioner);
  stats.add_edges({edge});
  edge = Utils::add_edge_or_update_weigth(3, 4, 1, *g, *partitioner);
  stats.add_edges({edge});
  ASSERT_EQ(stats.m_cc_size.size(), 2);
  ASSERT_EQ(stats.m_cc_size[1], 3);
  ASSERT_EQ(stats.m_cc_size[4], 2);

  edge = Utils::add_edge_or_update_weigth(5, 6, 1, *g, *partitioner);
  stats.add_edges({edge});
  ASSERT_EQ(stats.m_cc_size.size(), 3);
  ASSERT_EQ(stats.m_cc_size[6], 2);

  edge = Utils::add_edge_or_update_weigth(0, 5, 1, *g, *partitioner);
  stats.add_edges({edge});
  ASSERT_EQ(stats.m_cc_size.size(), 2);
  ASSERT_EQ(stats.m_cc_size[6], 5);
}

TEST_F(GraphTest, comutesCorrectEdgeCut) {
  Edge edge;
  vector<pair<uint32_t, uint32_t>> part = {
      {0, 0}, {1, 0}, {2, 0}, {3, 1}, {4, 1}};
  for (const auto &p : part)
    (*partitioning)[p.first] = p.second;

  std::vector<std::tuple<Edge, Utils::EDGE_PROP>> involved_edges;

  vector<pair<uint32_t, uint32_t>> edges = {
      {0, 1}, {0, 2}, {0, 3}, {0, 4}, {3, 4}};
  for (const auto &edge : edges)
    involved_edges.push_back(Utils::add_edge_or_update_weigth(
        edge.first, edge.second, 1, *g, *partitioner));
        
  partitioner->define_partitioning(*partitioning);
  auto stats = Statistics(*g, *partitioner, *config);
  stats.add_edges(involved_edges);
  ASSERT_EQ(stats.m_edges_cut, 2);
}