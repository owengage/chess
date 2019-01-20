add_subdirectory(text)
add_subdirectory(test)

add_library(chess
        loc.cpp board.cpp text/print.cpp game.cpp test/move_pawn_test.cpp)