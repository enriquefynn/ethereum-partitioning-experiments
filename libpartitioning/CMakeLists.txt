INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/libpartitioning/include
  ${CMAKE_SOURCE_DIR}/libcommon/include
  ${CMAKE_SOURCE_DIR}/libutils/include
  ${LIBEVENT_INCLUDE_DIRS}
  )

SET(SRCS
  src/fb_partitioning.cpp
  src/file_partitioning.cpp
  src/hash_partitioning.cpp
  src/HMETIS_partitioning.cpp
  src/METIS_partitioning.cpp
  src/partitioner.cpp)

ADD_LIBRARY(partitioning SHARED ${SRCS})
TARGET_LINK_LIBRARIES(partitioning ${LIBMETIS_LIBRARIES})

