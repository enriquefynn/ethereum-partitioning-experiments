
#include <gtest/gtest.h>

#include <boost/graph/graph_utility.hpp>
#include <config.h>
#include <fstream>
#include <hash_partitioning.h>
#include <unordered_map>
#include <utils.h>
#include <statistics.h>

typedef std::unordered_map<uint32_t, uint32_t> map_type;

using namespace std;
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
  Utils::add_edge_or_update_weigth(1, 1, 1, *g, *partitioner);
  Utils::add_edge_or_update_weigth(1, 1, 1, *g, *partitioner);

  ASSERT_EQ(1, boost::num_vertices(*g));
  ASSERT_EQ(0, boost::num_edges(*g));
}

TEST_F(GraphTest, haveEdgeWeight) {
  Edge edge;
  tie(edge, std::ignore) =
      Utils::add_edge_or_update_weigth(1, 10, 10, *g, *partitioner);
  Utils::add_edge_or_update_weigth(1, 10, 1, *g, *partitioner);
  Utils::add_edge_or_update_weigth(1, 10, 1, *g, *partitioner);

  auto weights_map = get(boost::edge_weight, *g);
  auto weight = get(weights_map, edge);

  ASSERT_EQ(12, weight);
}

TEST_F(GraphTest, haveVertexWeight) {
  Edge edge;
  tie(edge, std::ignore) =
      Utils::add_edge_or_update_weigth(1, 10, 10, *g, *partitioner);
  Utils::add_edge_or_update_weigth(1, 10, 1, *g, *partitioner);
  Utils::add_edge_or_update_weigth(1, 10, 1, *g, *partitioner);

  ASSERT_EQ((*g)[boost::target(edge, *g)].m_vertex_weight, 3);
  ASSERT_EQ((*g)[boost::source(edge, *g)].m_vertex_weight, 3);
}

TEST_F(GraphTest, ComputesCorrectCC) {
  config->GRAPH_CC_PATH = "/tmp/cc.txt";
  auto stats = Statistics(*g, *partitioner, *config);

  Edge edge;
  Utils::add_edge_or_update_weigth(0, 1, 1, *g, *partitioner);
  ASSERT_EQ(stats.m_number_of_cc, 1);
  Utils::add_edge_or_update_weigth(1, 2, 1, *g, *partitioner);
  Utils::add_edge_or_update_weigth(3, 4, 1, *g, *partitioner);
  ASSERT_EQ(stats.m_number_of_cc, 2);
  Utils::add_edge_or_update_weigth(5, 6, 1, *g, *partitioner);
  ASSERT_EQ(stats.m_number_of_cc, 3);
  Utils::add_edge_or_update_weigth(0, 5, 1, *g, *partitioner);
  ASSERT_EQ(stats.m_number_of_cc, 2);
}