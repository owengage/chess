add_subdirectory(text)
add_subdirectory(test)

add_library(chess
        loc.cpp
        board.cpp
        game.cpp
        move_generator.cpp)