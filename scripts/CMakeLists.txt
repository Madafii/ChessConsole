add_executable(addToDB addToDB.cpp)
add_executable(convertPgnToMoves convertPgnToMoves.cpp)

target_link_libraries(addToDB chessML)
target_link_libraries(convertPgnToMoves chessML)
