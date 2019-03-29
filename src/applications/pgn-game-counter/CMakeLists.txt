add_executable(pgn-game-counter)

target_sources(pgn-game-counter
        PRIVATE
        main.cpp)

target_link_libraries(pgn-game-counter
        PRIVATE
        chess-pgn)