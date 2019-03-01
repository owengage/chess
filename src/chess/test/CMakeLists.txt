add_executable(move_test
        square_test.cpp
        move_pawn_test.cpp
        move_knight_test.cpp
        move_general_test.cpp
        move_rook_test.cpp
        move_bishop_test.cpp
        move_queen_test.cpp
        move_king_test.cpp)

target_link_libraries(move_test
        chess
        gmock
        gtest_main)

add_test(NAME move_test COMMAND move_test)