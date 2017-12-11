
#include <gtest/gtest.h>

#include <fbpartitioning.h>
#include <fstream>
#include <utils.h>

using namespace std;
class OracleTest : public ::testing::Test {
public:
  OracleTest() {}

protected:
  virtual void SetUp() {
    g = std::unique_ptr<Graph>(new Graph());
    partitioning =
        std::unique_ptr<std::vector<uint32_t>>(new std::vector<uint32_t>());

    std::ifstream test_graph("./test/src/fb_partitioning/test_graph.txt",
                             std::ifstream::in);
    uint32_t n_vtx, n_edges, fr, to;
    test_graph >> n_edges;
    for (int i = 0; i < n_edges; ++i) {
      test_graph >> fr >> to;
      Utils::add_edge_or_update_weigth(fr, to, 1, *g);
    }
    test_graph >> n_vtx;
    for (int i = 0; i < n_vtx; ++i) {
      test_graph >> fr;
      partitioning->push_back(fr);
    }
  }
  virtual void TearDown() {
    // do nothing
  }

protected:
  std::unique_ptr<Graph> g;
  std::unique_ptr<std::vector<uint32_t>> partitioning;
};

TEST_F(OracleTest, matrixTest) {
  // Graph g;

  auto fb_partitioner = FB_partitioner(*g);
  fb_partitioner.define_partitioning(std::move(*partitioning));

  auto matrix = fb_partitioner.get_oracle_matrix(4);

  std::vector<std::vector<double>> v({{0, 5 / 7., 1, 0},
                                      {1, 0, 1, 2 / 3.},
                                      {4 / 9., 8 / 10., 0, 1},
                                      {0, 1, 1, 0}});
  EXPECT_EQ(v, matrix);
}