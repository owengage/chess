add_subdirectory(text)
add_subdirectory(test)

add_library(chess
        loc.cpp
        board.cpp
        game.cpp
        square.cpp
        available_moves.cpp
        basic_driver.cpp
        )