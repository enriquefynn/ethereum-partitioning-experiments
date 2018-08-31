INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/libfuture/include
  ${CMAKE_SOURCE_DIR}/libpartitioning/include
  ${CMAKE_SOURCE_DIR}/libcommon/include
  ${CMAKE_SOURCE_DIR}/libutils/include
  )

SET(SRCS
  src/future_partitioning.cpp
  src/part_graph_partitioning.cpp)

ADD_LIBRARY(future SHARED ${SRCS})
TARGET_LINK_LIBRARIES(future partitioning)

