add_library(chess-bench INTERFACE)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(benchmark REQUIRED MODULE)

target_include_directories(chess-bench
        INTERFACE
        "${benchmark_INCLUDE_DIRS}")

# note main comes from looking for benchmark_main in the Findbenchmark.cmake module.
target_link_libraries(chess-bench
        INTERFACE
        "${benchmark_LIBRARY}"
        Threads::Threads)

