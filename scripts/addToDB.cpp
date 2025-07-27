#include "ChessData.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // if (argc != 3) {
    //     std::cerr << "from_file to_db" << std::endl;
    //     return 0;
    // }
    //
    // const std::string fromFileName = argv[1];
    // const std::string dbName = argv[2];
    const std::string fromFileName = "../data/lichess/outDataPGN/lichess_db_standard.rated_2013-01.txt";
    const std::string dbName = "chessMovesTest";

    ChessData chessData;
    chessData.pushMovesToDB(fromFileName, dbName);

    return 0;
}
