add_subdirectory(text)
add_subdirectory(pgn)
add_subdirectory(test)
add_subdirectory(bench)

add_library(chess)

target_include_directories(chess
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include)

target_sources(chess
        PRIVATE
        Loc.cpp
        Board.cpp
        Game.cpp
        available_moves.cpp
        BasicDriver.cpp
        Suggester.cpp)

target_link_libraries(chess
        chess-perf)