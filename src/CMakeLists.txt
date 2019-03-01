add_subdirectory(chess)

add_executable(play)

target_sources(play
        PRIVATE
        main.cpp)

target_link_libraries(play
        PRIVATE
        chess
        chess-text)