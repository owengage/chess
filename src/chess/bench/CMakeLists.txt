add_executable(available-moves-bench)

target_sources(available-moves-bench
        PRIVATE
        available_moves_bench.cpp)

target_link_libraries(available-moves-bench
        chess
        chess-pgn
        chess-bench)


add_executable(suggester-bench)

target_sources(suggester-bench
        PRIVATE
        suggester_bench.cpp)

target_link_libraries(suggester-bench
        chess
        chess-bench)