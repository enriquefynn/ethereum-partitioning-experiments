INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/libutils/include 
  ${CMAKE_SOURCE_DIR}/libcommon/include
  ${CMAKE_SOURCE_DIR}/libpartitioning/include
  )

SET(SRCS src/utils.cpp)

ADD_LIBRARY(utils SHARED ${SRCS})
TARGET_LINK_LIBRARIES(utils)

