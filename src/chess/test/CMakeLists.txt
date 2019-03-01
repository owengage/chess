add_executable(move_test)

target_include_directories(move_test
        PRIVATE
        "${gtest_SOURCE_DIR}/include"
        "${gmock_SOURCE_DIR}/include")

target_sources(move_test
        PRIVATE
        square_test.cpp
        move_pawn_test.cpp
        move_knight_test.cpp
        move_general_test.cpp
        move_rook_test.cpp
        move_bishop_test.cpp
        move_queen_test.cpp
        move_king_test.cpp)

target_link_libraries(move_test
        PRIVATE
        chess
        gmock
        gtest_main)

add_test(NAME move_test COMMAND move_test)