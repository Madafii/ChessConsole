add_library(chess 
    ChessInstance.h ChessInstance.cpp
    ChessPlayer.h ChessPlayer.cpp
    ChessPlayerConsoleHuman.h ChessPlayerConsoleHuman.cpp
    ChessPlayerAnalyzer.h ChessPlayerAnalyzer.cpp
    ChessPlayerRnd.h ChessPlayerRnd.cpp
    ChessPlayerDB.h ChessPlayerDB.cpp
    ChessUI.h ChessUI.cpp
    ChessConsoleUI.h ChessConsoleUI.cpp
)

target_include_directories(chess PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(chess chessBase chessML chessAnalyzer)

add_subdirectory(base)
add_subdirectory(ML)
add_subdirectory(Analyzer)
