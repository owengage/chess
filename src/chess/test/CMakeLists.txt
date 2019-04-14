#
# move_test
#

add_executable(move_test)

target_sources(move_test
        PRIVATE
        square_test.cpp
        move_pawn_test.cpp
        move_knight_test.cpp
        move_general_test.cpp
        move_rook_test.cpp
        move_bishop_test.cpp
        move_queen_test.cpp
        move_king_test.cpp
        available_moves_test.cpp)

target_link_libraries(move_test
        PRIVATE
        chess
        chess-test)

add_test(NAME move_test COMMAND move_test)

#
# suggester_test
#

add_executable(suggester_test)

target_sources(suggester_test
        PRIVATE
        suggester_test.cpp)

target_link_libraries(suggester_test
        PRIVATE
        chess
        chess-test)

add_test(NAME suggester_test COMMAND suggester_test)