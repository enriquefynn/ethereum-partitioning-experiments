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
    if (key == "METIS") {
      if (value == "DEFAULT")
        METIS_MODE = DEFAULT_METIS;
      else if (value == "HMETIS")
        METIS_MODE = HMETIS;
      else
        assert(false);
    } else if (key == "REPARTITION") {
      if (value == "PERIODIC")
        PARTITIONING_MODE = PERIODIC_PARTITIONING;
      else if (value == "DYNAMIC")
        PARTITIONING_MODE = DYNAMIC_PARTITIONING;
      else
        assert(false);
    }
  }
}

const std::string Config::get_partitioning_mode() {
  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << CROSS_PARTITION_THRESHOLD;
  std::string threshold = stream.str();

  return (PARTITIONING_MODE == PERIODIC_PARTITIONING)
             ? "PERIODIC"
             : "DYNAMIC_" + threshold + "_WINDOW_" + std::to_string(TIME_REPARTITION_WINDOW);
}

const std::string Config::get_METIS_name() {
  return ((METIS_MODE == DEFAULT_METIS) ? "METIS" : "HMETIS") +
         std::string("_") + get_partitioning_mode();
}