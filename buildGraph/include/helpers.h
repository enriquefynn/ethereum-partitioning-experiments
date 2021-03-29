#pragma once
#include <partitioner.h>

#include <HMETIS_partitioning.h>
#include <METIS_partitioning.h>
#include <config.h>
#include <fb_partitioning.h>
#include <file_partitioning.h>
#include <hash_partitioning.h>

#include <future_partitioning.h>
#include <part_graph_partitioning.h>

namespace GraphHelpers {
std::unique_ptr<Partitioner> get_partitioner(Graph &graph, Config &config) {
  switch (config.PARTITIONING_MODE) {
  case Config::HASH_PARTITIONER:
    return std::unique_ptr<Partitioner>(new Hash_partitioner(graph, config));
  case Config::METIS_PARTITIONER:
    return std::unique_ptr<Partitioner>(new METIS_partitioner(graph, config));
  case Config::HMETIS_PARTITIONER:
    return std::unique_ptr<Partitioner>(new HMETIS_partitioner(graph, config));
  case Config::FACEBOOK_PARTITIONER:
    return std::unique_ptr<Partitioner>(new FB_partitioner(graph, config));
  case Config::FILE_PARTITIONER:
    return std::unique_ptr<Partitioner>(new File_partitioner(graph, config));
  case Config::FUTURE_PARTITIONER:
    return std::unique_ptr<Partitioner>(new Future_partitioner(graph, config));
  case Config::PART_GRAPH_PARTITIONER:
    return std::unique_ptr<Partitioner>(
        new Part_graph_partitioner(graph, config));
  default:
    assert(false);
    break;
  }
}
} // namespace GraphHelpers