add_subdirectory(test)

add_library(chess-perf)

target_include_directories(chess-perf
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include)

message(STATUS "Source dir: ${CMAKE_SOURCE_DIR}")

target_sources(chess-perf
        PRIVATE
        StackVector.cpp)