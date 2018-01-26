
#include <gtest/gtest.h>

#include <fb_partitioning.h>
#include <fstream>
#include <statistics.h>
#include <unordered_map>
#include <utils.h>

typedef std::unordered_map<uint32_t, uint32_t> map_type;

using namespace std;
class OracleTest : public ::testing::Test {
public:
  OracleTest() {}

protected:
  virtual void SetUp() {
    g = std::unique_ptr<Graph>(new Graph());
    config = std::unique_ptr<Config>(
        new Config("./test/src/fb_partitioning/test_config.txt"));

    partitioning = std::unique_ptr<map_type>(new map_type());
    partitioner =
        std::unique_ptr<FB_partitioner>(new FB_partitioner(*g, *config));
    stats =
        std::unique_ptr<Statistics>(new Statistics(*g, *partitioner, *config));

    std::ifstream test_graph("./test/src/fb_partitioning/test_graph.txt",
                             std::ifstream::in);
    uint32_t n_vtx, n_edges, fr, to;
    test_graph >> n_edges;
    for (int i = 0; i < n_edges; ++i) {
      test_graph >> fr >> to;
      Utils::add_edge_or_update_weigth(fr, to, 1, *g, *partitioner);
    }
    test_graph >> n_vtx;
    for (int i = 0; i < n_vtx; ++i) {
      test_graph >> fr;
      (*partitioning)[i] = fr;
    }
  }
  virtual void TearDown() {
    // do nothing
  }

protected:
  std::unique_ptr<Graph> g;
  std::unique_ptr<map_type> partitioning;
  std::unique_ptr<Config> config;
  std::unique_ptr<FB_partitioner> partitioner;
  std::unique_ptr<Statistics> stats;
};

TEST_F(OracleTest, matrixTest) {

  partitioner->define_partitioning(std::move(*partitioning));

  auto matrix = partitioner->get_oracle_matrix(4);

  std::vector<std::vector<double>> v({{0, 5 / 7., 1, 0},
                                      {1, 0, 1, 2 / 3.},
                                      {4 / 9., 8 / 10., 0, 1},
                                      {0, 1, 1, 0}});
  EXPECT_EQ(v, matrix);
}

// TEST_F(OracleTest, testMoving) {
//   // Graph g;

//   auto fb_partitioner = FB_partitioner(*g, *config);
//   fb_partitioner.define_partitioning(std::move(*partitioning));

//   auto matrix = fb_partitioner.partition(4);

//   // EXPECT_EQ(v, matrix);
// }