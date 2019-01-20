add_subdirectory(chess)

add_executable(play
        main.cpp ../include/chess/text/print.h ../include/chess/game.h)

target_link_libraries(play
        chess
        chess-text)