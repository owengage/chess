add_executable(pgn_test)

target_sources(pgn_test
        PRIVATE
        lexer_test.cpp
        move_parser_test.cpp
        resolve_move_test.cpp
        validate_test.cpp)

target_include_directories(pgn_test
        PRIVATE
        "${gtest_SOURCE_DIR}/include"
        "${gmock_SOURCE_DIR}/include")

target_link_libraries(pgn_test
        chess-pgn
        gmock
        gtest_main)

add_test(NAME pgn::pgn_test COMMAND pgn_test)