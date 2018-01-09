#include <fstream>
#include <iomanip>
#include <sstream>

#include <config.h>
#include <log.h>

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
    } else if (key == "GRAPH_CC_PATH") {
      GRAPH_CC_PATH = value;
    } else if (key == "AVOID_PRECOMPILED") {
      AVOID_PRECOMPILED = (value == "true") ? true : false;
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
std::ostream &operator<<(std::ostream &os, const Config &c) {
  os << ANSI_COLOR_RED << "Using ";
  if (c.PARTITIONING_MODE == Config::HASH_PARTITIONER)
    os << "Hash ";
  else if (c.PARTITIONING_MODE == Config::HMETIS_PARTITIONER)
    os << "HMETIS ";
  else if (c.PARTITIONING_MODE == Config::METIS_PARTITIONER)
    os << "METIS ";
  else if (c.PARTITIONING_MODE == Config::FACEBOOK_PARTITIONER)
    os << "Facebook ";
  else if (c.PARTITIONING_MODE == Config::FILE_PARTITIONER)
    os << "File ";
  os << "partitioning\n";
  os << c.N_PARTITIONS << " partitions\n";
  if (c.SAVE_PARTITIONING)
    os << "Saving/using partitioning file " << c.FILE_PATH << '\n';
  os << "Graph size path: "
     << ((c.GRAPH_SIZE_EVOLUTION_PATH == "") ? "NULL"
                                             : c.GRAPH_SIZE_EVOLUTION_PATH);
  os << "\nAVOID PRECOMPILED "
     << ((c.AVOID_PRECOMPILED) ? "true\n" : "false\n");
  return os;
}