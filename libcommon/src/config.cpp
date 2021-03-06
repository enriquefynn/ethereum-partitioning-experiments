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
      else if (value == "FUTURE")
        PARTITIONING_MODE = FUTURE_PARTITIONER;
      else if (value == "PART_GRAPH")
        PARTITIONING_MODE = PART_GRAPH_PARTITIONER;
      else
        assert(false);
    } else if (key == "TIME_REPARTITION") {
      TIME_REPARTITION = std::stoul(value);
    } else if (key == "TIME_REPARTITION_WINDOW") {
      TIME_REPARTITION_WINDOW = std::stoul(value);
    } else if (key == "CROSS_PARTITION_THRESHOLD") {
      CROSS_PARTITION_THRESHOLD = std::stof(value);
    } else if (key == "BALANCE_THRESHOLD") {
      BALANCE_THRESHOLD = std::stof(value);
    } else if (key == "PARTITIONING_TYPE") {
      if (value == "PERIODIC_PARTITIONING")
        PARTITIONING_TYPE = PERIODIC_PARTITIONING;
      else if (value == "DYNAMIC_PARTITIONING")
        PARTITIONING_TYPE = DYNAMIC_PARTITIONING;
      else
        assert(false);
    } else if (key == "DELETE_VERTICES") {
      DELETE_VERTICES = (value == "true") ? true : false;
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
  else if (c.PARTITIONING_MODE == Config::FUTURE_PARTITIONER)
    os << "Future ";
  else if (c.PARTITIONING_MODE == Config::PART_GRAPH_PARTITIONER)
    os << "Partial graph ";
  
  os << "partitioning\n" << c.N_PARTITIONS << " partitions\n";
  if (c.PARTITIONING_TYPE == Config::PERIODIC_PARTITIONING) {
    os << "Periodic: " << c.TIME_REPARTITION << '\n';
  } else if (c.PARTITIONING_TYPE == Config::DYNAMIC_PARTITIONING) {
    os << "Dynamic\n\tWindow: " << c.TIME_REPARTITION_WINDOW
       << "\n\tCross partition threshold: " << std::fixed
       << std::setprecision(2) << c.CROSS_PARTITION_THRESHOLD
       << "\n\tBalance threshold:" << c.BALANCE_THRESHOLD << '\n';
  }

  if (c.SAVE_PARTITIONING)
    os << "Saving/using partitioning file " << c.FILE_PATH << '\n';
  os << "Graph size path: "
     << ((c.GRAPH_SIZE_EVOLUTION_PATH == "") ? "NULL"
                                             : c.GRAPH_SIZE_EVOLUTION_PATH);
  os << "\nAVOID PRECOMPILED "
     << ((c.AVOID_PRECOMPILED) ? "true\n" : "false\n");
  return os;
}