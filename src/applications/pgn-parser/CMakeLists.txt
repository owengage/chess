add_executable(pgn-parser)

target_sources(pgn-parser
        PRIVATE
        main.cpp)

target_link_libraries(pgn-parser
        PRIVATE
        chess-pgn)