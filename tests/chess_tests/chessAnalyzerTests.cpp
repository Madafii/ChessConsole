#include "ChessAnalyzer.h"
#include "ChessInterface.h"
#include "ChessPiece.h"
#include "gtest/gtest.h"
#include <algorithm>
#include <iterator>
#include <utility>

// match types in tiles vector
bool match(std::vector<const ChessTile *> tiles, const std::vector<ChessPieceType> &types) {
    if (tiles.size() != types.size()) return false;

    std::vector<std::pair<const ChessTile *, bool>> tilesFound;
    std::transform(tiles.begin(), tiles.end(), std::back_inserter(tilesFound),
                   [](const ChessTile *tile) { return std::make_pair(tile, false); });

    for (const ChessPieceType type : types) {
        auto found = std::ranges::find_if(tilesFound, [type](const std::pair<const ChessTile *, bool> &tilePair) {
            return tilePair.first->hasPiece(type) && !tilePair.second;
        });
        if (found == tilesFound.end()) return false;
    }
    return true;
}

TEST(evalTests, evalBoardValue) {
    ChessInterface chessInterface;
    ChessAnalyzer chessAna(chessInterface.getChessBoard());

    // check values with no moves made
    EXPECT_EQ(chessAna.evalBoardValue(true), 20);
    EXPECT_EQ(chessAna.evalBoardValue(false), 20);

    // make some moves then check board value
    chessInterface.handleInput("e2:e4"); // raise value by one
    chessInterface.handleInput("b8:a6"); // no change
    EXPECT_EQ(chessAna.evalBoardValue(true), 22);
    EXPECT_EQ(chessAna.evalBoardValue(false), 20);

    // some more inputs
    chessInterface.handleInput("e4:e5"); // no change
    chessInterface.handleInput("f7:f5"); // raise value by one
    chessInterface.handleInput("b1:c3"); // raise value by two
    chessInterface.handleInput("a6:c5"); // raise value by two
    EXPECT_EQ(chessAna.evalBoardValue(true), 24);
    EXPECT_EQ(chessAna.evalBoardValue(false), 23);
}

TEST(evalTests, evalPieceValue) {
    ChessInterface chessInterface;
    ChessAnalyzer chessAna(chessInterface.getChessBoard());

    // check values with no moves made
    EXPECT_EQ(chessAna.evalPieceValue(true), 39);
    EXPECT_EQ(chessAna.evalPieceValue(false), 39);

    // capture black piece
    chessInterface.handleInput("d2:d3");
    chessInterface.handleInput("d7:d5");
    chessInterface.handleInput("c1:f4");
    chessInterface.handleInput("c7:c6");
    chessInterface.handleInput("f4:b8");
    EXPECT_EQ(chessAna.evalPieceValue(true), 39);  // remains same
    EXPECT_EQ(chessAna.evalPieceValue(false), 36); // lost Knight -> lose 3 points

    // capture white piece
    chessInterface.handleInput("c8:g4");
    chessInterface.handleInput("e2:e3");
    chessInterface.handleInput("g4:d1");
    EXPECT_EQ(chessAna.evalPieceValue(true), 30);  // lost Queen -> lose 9 points
    EXPECT_EQ(chessAna.evalPieceValue(false), 36); // remains same
}

TEST(evalTests, evalKingMove) {
    ChessBoard board;
    ChessAnalyzer chessAna(board);
    const ChessTile &kingTile = board.getTileAt(4);
    // encourage castle move
    EXPECT_GT(chessAna.evalKingMoves(kingTile, board.getTileAt(6)), 0);
    // discourage king move if castling still possilbe
    EXPECT_LT(chessAna.evalKingMoves(kingTile, board.getTileAt(5)), 0);
    EXPECT_LT(chessAna.evalKingMoves(kingTile, board.getTileAt(3)), 0);
}

TEST(evalTests, evalRookMove) {
    ChessInterface chessInterface;
    const ChessBoard &board = chessInterface.getChessBoard();
    ChessAnalyzer chessAna(board);
    const ChessTile &leftRookTile = board.getTileAt(0);
    const ChessTile &rightRookTile = board.getTileAt(7);
    // discourage moving rook if castling still possible
    EXPECT_LT(chessAna.evalRookMoves(leftRookTile), 0);
    EXPECT_LT(chessAna.evalRookMoves(rightRookTile), 0);

    chessInterface.handleMoveInput("b1:c3");
    chessInterface.handleMoveInput("a7:a6");
    chessInterface.handleMoveInput("a1:b1");
    chessInterface.handleMoveInput("a6:a5");
    chessInterface.handleMoveInput("b1:a1");

    // left Rook already moved once so no encourage or discouragement for moving it
    EXPECT_EQ(chessAna.evalRookMoves(leftRookTile), 0);
}

TEST(matrixTests, defendMatrix) {
    ChessInterface chessInterface;
    ChessAnalyzer chessAna(chessInterface.getChessBoard());

    // check base value
    const auto defenderMatrix = chessAna.getDefendedMatrix();
    EXPECT_EQ(defenderMatrix.at(0).first.size(), 0);
    EXPECT_EQ(defenderMatrix.at(30).first.size(), 0);
    // first pawn only defended by rook
    EXPECT_EQ(defenderMatrix.at(8).first.size(), 1);
    EXPECT_EQ(defenderMatrix.at(8).first.front()->getPieceType(), ChessPieceType::ROOK);
    // 4th pawn on white side
    EXPECT_EQ(defenderMatrix.at(11).first.size(), 4);
    EXPECT_TRUE(
        match(defenderMatrix.at(11).first, {ChessPieceType::QUEEN, ChessPieceType::KNIGHT, ChessPieceType::KING, ChessPieceType::BISHOP}));
    // 3rd pawn on black side only defended by queen
    EXPECT_EQ(defenderMatrix.at(50).second.size(), 1);
    EXPECT_EQ(defenderMatrix.at(50).second.front()->getPieceType(), ChessPieceType::QUEEN);
    // 4th pawn on black side
    EXPECT_EQ(defenderMatrix.at(51).second.size(), 4);
    EXPECT_TRUE(
        match(defenderMatrix.at(51).second, {ChessPieceType::QUEEN, ChessPieceType::KNIGHT, ChessPieceType::KING, ChessPieceType::BISHOP}));
    // white side not defending here
    EXPECT_EQ(defenderMatrix.at(50).first.size(), 0);
    EXPECT_EQ(defenderMatrix.at(51).first.size(), 0);
}

TEST(matrixTests, attackMatrix) {
    ChessInterface chessInterface;
    ChessAnalyzer chessAna(chessInterface.getChessBoard());

    // check base attack matrix. Should be empty
    auto attackMatrix = chessAna.getAttackedMatrix();
    // same as defender matrix
    EXPECT_EQ(attackMatrix.at(0).first.size(), 0);
    EXPECT_EQ(attackMatrix.at(30).first.size(), 0);
    EXPECT_EQ(attackMatrix.at(16).first.size(), 2); // empty tile attacked by pawn and knight
    EXPECT_TRUE(match(attackMatrix.at(16).first, {ChessPieceType::PAWN, ChessPieceType::KNIGHT}));
    EXPECT_EQ(attackMatrix.at(17).first.size(), 2); // empty tile attacked by two pawns
    EXPECT_TRUE(match(attackMatrix.at(17).first, {ChessPieceType::PAWN, ChessPieceType::PAWN}));

    // pawns attacking each other
    chessInterface.handleInput("e2:e4");
    chessInterface.handleInput("f7:f5");
    attackMatrix = chessAna.getAttackedMatrix();
    // black pawn attacking here
    EXPECT_EQ(attackMatrix.at(28).second.size(), 1);
    EXPECT_EQ(attackMatrix.at(28).second.front()->getPieceType(), ChessPieceType::PAWN);
    // white pawn attacking here
    EXPECT_EQ(attackMatrix.at(37).first.size(), 1);
    EXPECT_EQ(attackMatrix.at(37).first.front()->getPieceType(), ChessPieceType::PAWN);

    // try some more attacking tiles
    chessInterface.handleInput("d1:h5");
    attackMatrix = chessAna.getAttackedMatrix();
    // white queen attacking black king and two pawns
    EXPECT_EQ(attackMatrix.at(60).first.size(), 1);
    EXPECT_EQ(attackMatrix.at(60).first.front()->getPieceType(), ChessPieceType::QUEEN);
    EXPECT_TRUE(attackMatrix.at(60).first.front()->hasWhitePiece());
    EXPECT_EQ(attackMatrix.at(37).first.size(), 1);
    EXPECT_EQ(attackMatrix.at(37).first.front()->getPieceType(), ChessPieceType::QUEEN);
    EXPECT_TRUE(attackMatrix.at(37).first.front()->hasWhitePiece());
    EXPECT_EQ(attackMatrix.at(55).first.size(), 1);
    EXPECT_EQ(attackMatrix.at(55).first.front()->getPieceType(), ChessPieceType::QUEEN);
    EXPECT_TRUE(attackMatrix.at(55).first.front()->hasWhitePiece());
    // empty tile being attacked by white queen
    EXPECT_EQ(attackMatrix.at(38).first.size(), 1);
    EXPECT_EQ(attackMatrix.at(38).first.front()->getPieceType(), ChessPieceType::QUEEN);
    EXPECT_TRUE(attackMatrix.at(38).first.front()->hasWhitePiece());
    // tile attack blocked by pawn
    EXPECT_EQ(attackMatrix.at(36).first.size(), 0);

    chessInterface.handleInput("g7:g6");
    attackMatrix = chessAna.getAttackedMatrix();
    // black pawn attacking white queen
    EXPECT_EQ(attackMatrix.at(39).second.size(), 1);
    EXPECT_EQ(attackMatrix.at(39).second.front()->getPieceType(), ChessPieceType::PAWN);
    EXPECT_FALSE(attackMatrix.at(39).second.front()->hasWhitePiece());
    // king attacking empty tile
    EXPECT_EQ(attackMatrix.at(53).second.size(), 1);
    EXPECT_EQ(attackMatrix.at(53).second.front()->getPieceType(), ChessPieceType::KING);
    EXPECT_FALSE(attackMatrix.at(53).second.front()->hasWhitePiece());
    // pawn and knight attacking empty tile
    EXPECT_EQ(attackMatrix.at(45).second.size(), 2);
    EXPECT_TRUE(match(attackMatrix.at(45).second, {ChessPieceType::PAWN, ChessPieceType::KNIGHT}));
    EXPECT_FALSE(attackMatrix.at(45).second.front()->hasWhitePiece());
}

TEST(bestMovesTests, basicTests) {
    ChessInterface chessInterface;
    ChessAnalyzer chessAna(chessInterface.getChessBoard());

    chessInterface.handleMoveInput("d2:d4");
    chessInterface.handleMoveInput("d7:d5");
    chessInterface.handleMoveInput("c1:f4");
    chessInterface.handleMoveInput("b8:c6");
    chessInterface.handleMoveInput("e2:e3");
    chessInterface.handleMoveInput("g8:f6");
    chessInterface.handleMoveInput("b1:d2");
    chessInterface.handleMoveInput("e7:e6");
    chessInterface.handleMoveInput("f1:d3");

    const auto bestMoves = chessAna.getBestEvalMoves(1);
    // for (const auto &[value, move] : bestMoves) {
    //     std::cout << "The best move is: " << move << " with a value of: " << std::to_string(value) << std::endl;
    // }
}


