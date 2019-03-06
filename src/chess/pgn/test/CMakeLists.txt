add_executable(lexer_test)

target_sources(lexer_test
        PRIVATE
        lexer_test.cpp)

target_include_directories(lexer_test
        PRIVATE
        "${gtest_SOURCE_DIR}/include"
        "${gmock_SOURCE_DIR}/include")

target_link_libraries(lexer_test
        chess-pgn
        gmock
        gtest_main)

add_test(NAME pgn::lexer_test COMMAND lexer_test)