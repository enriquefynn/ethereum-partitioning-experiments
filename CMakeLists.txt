cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_TYPE Debug)

project(ethereum-partitioner CXX)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# place binaries and libraries according to GNU standards
include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

find_package(Boost
  1.65.0
  REQUIRED
  )
find_package(LIBMETIS REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# include(cmake/googletest.cmake)

# fetch_googletest(
#   ${PROJECT_SOURCE_DIR}/cmake
#   ${PROJECT_BINARY_DIR}/googletest
#   )

# enable_testing()

add_subdirectory(libutils)
add_subdirectory(libcommon)
add_subdirectory(libpartitioning)
add_subdirectory(libfuture)
add_subdirectory(buildGraph)
# add_subdirectory(test)
