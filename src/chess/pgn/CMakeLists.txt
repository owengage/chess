add_subdirectory(test)

add_library(chess-pgn)

target_include_directories(chess-pgn
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include)

target_sources(chess-pgn
        PRIVATE
        Lexer.cpp
        MoveParser.cpp
        resolve_move.cpp
        validate.cpp)

target_link_libraries(chess-pgn
        chess)