INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/libcommon/include
  ${CMAKE_SOURCE_DIR}/libutils/include
  ${CMAKE_SOURCE_DIR}/libpartitioning/include
  ${CMAKE_SOURCE_DIR}/libfuture/include
  ${LIBMETIS_INCLUDE_DIRS}
  )

add_executable(
  unit_tests
  src/main.cpp
  src/fb_partitioning/oracle_test.cpp
  src/graph/graph_test.cpp
  src/graph/partitioner_test.cpp
    )

  target_link_libraries(
    unit_tests
    gtest_main
    common
    partitioning
    future
  )

add_test(
  unit
    ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/unit_tests
)
