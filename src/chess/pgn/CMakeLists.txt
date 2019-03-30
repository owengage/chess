add_subdirectory(test)

add_library(chess-pgn)

target_include_directories(chess-pgn
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include)

target_sources(chess-pgn
        PRIVATE
        lexer.cpp
        move_parser.cpp
        resolve_move.cpp
        validate.cpp)

target_link_libraries(chess-pgn
        chess)