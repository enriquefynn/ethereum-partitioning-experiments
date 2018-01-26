#define NOLOG
#include <fstream>
#include <gtest/gtest.h>
#include <unordered_map>

#include <boost/graph/graph_utility.hpp>

#include <config.h>
#include <file_partitioning.h>
#include <future_partitioning.h>
#include <statistics.h>
#include <utils.h>

typedef std::unordered_map<uint32_t, uint32_t> map_type;

using namespace std;
class PartitionerTests : public ::testing::Test {
public:
  PartitionerTests() {}

protected:
  virtual void SetUp() {
    g = std::unique_ptr<Graph>(new Graph());
    fut_config =
        std::unique_ptr<Config>(new Config("./test/src/graph/test_config.txt"));
    file_config =
        std::unique_ptr<Config>(new Config("./test/src/graph/test_config.txt"));

    stats = std::unique_ptr<Statistics>(
        new Statistics(*g, *future_partitioner, *fut_config));
  }
  virtual void TearDown() {
    // do nothing
  }

protected:
  std::unique_ptr<Graph> g;
  std::unique_ptr<Config> fut_config;
  std::unique_ptr<Config> file_config;
  std::unique_ptr<Future_partitioner> future_partitioner;
  std::unique_ptr<Statistics> stats;
};

TEST_F(PartitionerTests, saveFuturePartitioning) {
  fut_config->SAVE_PARTITIONING = true;
  fut_config->FILE_INPUT = std::fstream("/tmp/part.txt", std::ios::out);
  future_partitioner = std::unique_ptr<Future_partitioner>(
      new Future_partitioner(*g, *fut_config));

  auto vertices = std::vector<std::pair<uint32_t, uint32_t>>(
      {{0, 1}, {1, 2}, {0, 2}, {3, 4}, {5, 4}, {3, 5}, {0, 3}});

  for (auto const vertex : vertices)
    Utils::add_edge_or_update_weigth(vertex.first, vertex.second, 1, *g,
                                     *future_partitioner);

  future_partitioner->assign_partition({0, 1, 2, 3, 4, 5}, 2);
  future_partitioner->trigger_partitioning(10000000, 1, 1, {0, 0});
  future_partitioner->partition(2);
  // Speculate on METIS performance

  std::for_each(vertices.begin(), vertices.end(),
                [](std::pair<uint32_t, uint32_t> &p) {
                  p.first += 6;
                  p.second += 6;
                });
  for (auto const vertex : vertices)
    Utils::add_edge_or_update_weigth(vertex.first, vertex.second, 1, *g,
                                     *future_partitioner);
  future_partitioner->assign_partition({6, 7, 8, 9, 10, 11}, 2);
  future_partitioner->trigger_partitioning(100000000, 1, 1, {0, 0});
  future_partitioner->partition(2);
}

TEST_F(PartitionerTests, readFromFuturePartitioning) {
  uint32_t epoch, n_vtx, vtx, part;
  auto part_file = std::fstream("/tmp/part.txt", std::ios::in);
  std::vector<map_type> fut_partitionings;
  while (part_file >> epoch >> n_vtx) {
    map_type partitioning;
    while (n_vtx--) {
      part_file >> vtx >> part;
      partitioning[vtx] = part;
    }
    fut_partitionings.push_back(partitioning);
  }

  file_config->SAVE_PARTITIONING = true;
  file_config->FILE_INPUT = std::fstream("/tmp/part.txt", std::ios::in);

  auto file_partitioner = new File_partitioner(*g, *file_config);
  ASSERT_EQ(file_partitioner->m_partitioning, fut_partitionings.at(0));

  file_partitioner->trigger_partitioning(10000000, 1, 1, {0, 0});
  file_partitioner->partition(2);
  ASSERT_EQ(file_partitioner->m_partitioning, fut_partitionings.at(1));
}