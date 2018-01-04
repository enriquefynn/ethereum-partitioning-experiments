// #include <HMETIS_methods.h>
// #include <METIS_methods.h>
// #include <config.h>
// #include <fbpartitioning.h>
// #include <file_partitioning.h>
// #include <hash_partitioning.h>
// #include <log.h>
// #include <utils.h>

// namespace Utils {
// std::unique_ptr<Partitioner> get_partitioner(const Graph &graph,
//                                              Config &config) {
//   std::unique_ptr<Partitioner> partitioner;
//   switch (config.PARTITIONING_MODE) {
//   case Config::HASH_PARTITIONER:
//     LOG_INFO("Using Hash partitioner");
//     partitioner =
//         std::unique_ptr<Partitioner>(new Hash_partitioner(graph, config));
//     break;
//   case Config::METIS_PARTITIONER:
//     LOG_INFO("Using METIS partitioner");
//     partitioner =
//         std::unique_ptr<Partitioner>(new METIS_partitioner(graph, config));
//     break;
//   case Config::HMETIS_PARTITIONER:
//     LOG_INFO("Using Hyper-METIS partitioner");
//     partitioner =
//         std::unique_ptr<Partitioner>(new HMETIS_partitioner(graph, config));
//     break;
//   case Config::FACEBOOK_PARTITIONER:
//     LOG_INFO("Using Facebook partitioner");
//     partitioner =
//         std::unique_ptr<Partitioner>(new FB_partitioner(graph, config));
//     break;
//   case Config::FILE_PARTITIONER:
//     LOG_INFO("Using File partitioner");
//     partitioner =
//         std::unique_ptr<Partitioner>(new File_partitioner(graph, config));
//     break;

//   default:
//     assert(false);
//     break;
//   }
//   return partitioner;
// }
// } // namespace Utils