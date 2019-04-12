add_library(chess-test INTERFACE)

target_include_directories(chess-test
        INTERFACE
        "${gtest_SOURCE_DIR}/include"
        "${gmock_SOURCE_DIR}/include")

target_link_libraries(chess-test
        INTERFACE
        gmock
        gtest_main)