#
# StackVector
#

add_executable(StackVector_test)

target_sources(StackVector_test
        PRIVATE
        StackVector_test.cpp)

target_link_libraries(StackVector_test
        PRIVATE
        chess-perf
        chess-test)

add_test(NAME StackVector_test COMMAND StackVector_test)