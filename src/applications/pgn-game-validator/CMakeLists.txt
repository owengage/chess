add_executable(pgn-game-validator)

target_sources(pgn-game-validator
        PRIVATE
        main.cpp)

target_link_libraries(pgn-game-validator
        PRIVATE
        chess
        chess-pgn)