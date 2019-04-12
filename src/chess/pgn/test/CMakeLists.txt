add_executable(pgn_test)

target_sources(pgn_test
        PRIVATE
        lexer_test.cpp
        move_parser_test.cpp
        resolve_move_test.cpp
        validate_test.cpp)

target_link_libraries(pgn_test
        chess-pgn
        chess-test)

add_test(NAME pgn::pgn_test COMMAND pgn_test)