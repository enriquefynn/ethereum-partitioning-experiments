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
    }
    else if (key == "FILEPATH") {
      FILE_PATH = value;
    }
    else 
      assert(false);
  }
}
