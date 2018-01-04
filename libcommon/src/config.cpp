#include <fstream>
#include <iomanip>
#include <sstream>

#include "config.h"

Config::Config(const std::string &input_config) {
  std::ifstream config_file(input_config);
  std::string key, value;
  while (config_file >> key >> value) {
    if (key[0] == '#')
      continue;
    if (key == "PARTITIONING") {
      if (value == "HASH")
        PARTITIONING_MODE = HASH_PARTITIONER;
      else if (value == "METIS")
        PARTITIONING_MODE = METIS_PARTITIONER;
      else if (value == "HMETIS")
        PARTITIONING_MODE = HMETIS_PARTITIONER;
      else if (value == "FACEBOOK")
        PARTITIONING_MODE = FACEBOOK_PARTITIONER;
      else if (value == "FILE")
        PARTITIONING_MODE = FILE_PARTITIONER;
      else
        assert(false);
    } else if (key == "FILEPATH") {
      SAVE_PARTITIONING = true;
      FILE_PATH = value;
    } else if (key == "N_PARTITIONS") {
      N_PARTITIONS = std::stoul(value);
    } else if (key == "GRAPH_SIZE_EVOLUTION_PATH") {
      GRAPH_SIZE_EVOLUTION_PATH = value;
    } else
      assert(false);
  }
  if (SAVE_PARTITIONING) {
    if (PARTITIONING_MODE == FILE_PARTITIONER)
      FILE_INPUT = std::fstream(value, std::ios::in);
    else
      FILE_INPUT = std::fstream(value, std::ios::out);
  }
}

bool operator==(const VertexProperty &lhs, const VertexProperty &rhs) {
  return lhs.m_vertex_id == lhs.m_vertex_id;
}
bool operator!=(const VertexProperty &lhs, const VertexProperty &rhs) {
  return lhs.m_vertex_id != lhs.m_vertex_id;
}
std::ostream &operator<<(std::ostream &os, const VertexProperty &p) {
  os << p.m_vertex_id;
  return os;
}