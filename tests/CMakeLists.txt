add_executable(chess_input chess_tests/chessInputTests.cpp)
add_executable(chess_dbInterface chess_tests/chessDatabaseInterfaceTests.cpp)
add_executable(chess_analyzer chess_tests/chessAnalyzerTests.cpp)

target_link_libraries(chess_input chess gtest_main)
target_link_libraries(chess_dbInterface chess chessML gtest_main)
target_link_libraries(chess_analyzer chessBase chessAnalyzer gtest_main)

# deprecated now using gtest_discover_tests add_test(NAME chess_input_test
# COMMAND chess_input) add_test(NAME chess_dbInterface_test COMMAND
# chess_dbInterface)

add_subdirectory(lib/googletest)

include(GoogleTest)

gtest_discover_tests(chess_input)
gtest_discover_tests(chess_dbInterface)
gtest_discover_tests(chess_analyzer)
