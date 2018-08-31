INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/libcommon/include
  ${CMAKE_SOURCE_DIR}/libutils/include
  ${CMAKE_SOURCE_DIR}/libpartitioning/include
  )

SET(SRCS src/config.cpp src/statistics.cpp)

ADD_LIBRARY(common SHARED ${SRCS})
TARGET_LINK_LIBRARIES(common)

