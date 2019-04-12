cmake_minimum_required(VERSION 3.13)
project(chess)

set(CMAKE_CXX_STANDARD 17)

include(cmake-config/configure-gtest.cmake)
include(cmake-config/chess-test.target.cmake)

enable_testing()
add_subdirectory(src)