#pragma once
#include <partitioner.h>

#include <METIS_partitioning.h>
#include <HMETIS_partitioning.h>
#include <config.h>
#include <fb_partitioning.h>
#include <file_partitioning.h>
#include <future_partitioning.h>
#include <hash_partitioning.h>

namespace GraphHelpers {
std::unique_ptr<Partitioner> get_partitioner(Graph &graph,
                                             Config &config) {
  std::unique_ptr<Partitioner> partitioner;
  switch (config.PARTITIONING_MODE) {
  case Config::HASH_PARTITIONER:
    partitioner =
        std::unique_ptr<Partitioner>(new Hash_partitioner(graph, config));
    break;
  case Config::METIS_PARTITIONER:
    partitioner =
        std::unique_ptr<Partitioner>(new METIS_partitioner(graph, config));
    break;
  case Config::HMETIS_PARTITIONER:
    partitioner =
        std::unique_ptr<Partitioner>(new HMETIS_partitioner(graph, config));
    break;
  case Config::FACEBOOK_PARTITIONER:
    partitioner =
        std::unique_ptr<Partitioner>(new FB_partitioner(graph, config));
    break;
  case Config::FILE_PARTITIONER:
    partitioner =
        std::unique_ptr<Partitioner>(new File_partitioner(graph, config));
    break;
  case Config::FUTURE_PARTITIONER:
    partitioner =
        std::unique_ptr<Partitioner>(new Future_partitioner(graph, config));
    break;
  default:
    assert(false);
    break;
  }
  return partitioner;
}
} // namespace GraphHelpers