add_executable(move_test
        move_pawn_test.cpp
        move_knight_test.cpp
        move_general_test.cpp)

target_link_libraries(move_test
        chess
        gtest_main)

add_test(NAME move_test COMMAND move_test)