cmake_minimum_required(VERSION 3.13)
project(chess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-config/modules/")

include(cmake-config/configure-gtest.cmake)
include(cmake-config/chess-test.target.cmake)
include(cmake-config/chess-bench.target.cmake)

enable_testing()
add_subdirectory(src)