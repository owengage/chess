add_subdirectory(text)
add_subdirectory(test)

add_library(chess)

target_include_directories(chess
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include)

target_sources(chess
        PRIVATE
        loc.cpp
        board.cpp
        game.cpp
        available_moves.cpp
        basic_driver.cpp)