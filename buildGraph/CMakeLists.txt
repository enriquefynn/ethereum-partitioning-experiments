INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/buildGraph/include
  ${CMAKE_SOURCE_DIR}/libpartitioning/include
  ${CMAKE_SOURCE_DIR}/libfuture/include
  ${CMAKE_SOURCE_DIR}/libcommon/include
  ${CMAKE_SOURCE_DIR}/libutils/include
  ${LIBMETIS_INCLUDE_DIRS}
  )

add_executable(buildGraph src/buildGraph.cpp)
target_link_libraries(buildGraph partitioning future common utils)
