include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(runChessTests
        chessInputTests.cpp
)

target_link_libraries(runChessTests gtest gtest_main)
target_link_libraries(runChessTests chessBase)