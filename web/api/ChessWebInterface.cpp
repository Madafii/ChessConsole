#include "ChessWebInterface.h"
#include "ChessAnalyzer.h"
#include "ChessBoard.h"
#ifdef DATABASE
#include "ChessDatabaseInterface.h"
#endif
#include "ChessInterface.h"
#include "ChessPeepo.h"
#include "httplib.h"
// #include "nlohmann/json.hpp"

using namespace httplib;
// using nlohmann::json;

ChessWebInterface::ChessWebInterface() {
    // for testing create one chessInstance. I guess that makes it for every user the same board. So gotta learn how every user has its own
    // instance
    ChessInterface chessInterface;
#ifdef DATABASE
    ChessDatabaseInterface chessDB("chessMoves");
#endif

    GameState game_state = GameState::IN_PROGRESS;
    // will point to base board without moves
    table_pair gameDepth(0, false);
    int fromMoveId = 1;
    bool analyzer = false;

    httplib::Server svr;

    // auto ret = svr.set_mount_point("/", "/app/www");
    auto ret = svr.set_mount_point("/", "./www"); // for localhost
    if (!ret) {
        std::cerr << "the specified mount point does not exist" << std::endl;
    }

    svr.Get("/reset", [&](const Request &req, Response &res) {
        chessInterface = ChessInterface();
        game_state = GameState::IN_PROGRESS;
        gameDepth = {0, false};
        fromMoveId = 1;
        analyzer = false;
        std::cout << "reset chess game" << std::endl;
        res.status = 200;
        res.set_content("game reseted", "text/plain");
    });

    svr.Post("/drag-start", [&](const Request &req, Response &res) {
        const std::string fromTile = req.body;
        std::cout << "fromTile: " << fromTile << std::endl;
        const auto possMoves = chessInterface.handleFromInput(fromTile);
        if (!possMoves) {
            // TODO: response for invalid input
            res.status = 400;
            res.set_content("error", "text/plain");
            return;
        }

        std::string resPossMoves;
        for (const auto &possMove : *possMoves) {
            resPossMoves.append(possMove->getMove() + " ");
        }
        if (resPossMoves.size() > 0) {
            resPossMoves.erase(resPossMoves.size() - 1);
        }

        res.status = 200;
        res.set_content(resPossMoves, "text/plain");
    });

    // make player move and db move
    svr.Post("/drag-end", [&](const Request &req, Response &res) {
        const std::string toTile = req.body;
        std::cout << "move to tile: " << toTile << std::endl;
        const auto gameState = chessInterface.handleMoveInput(toTile);
        if (!gameState) {
            res.status = 400;
            res.set_content("could not make move", "text/plain");
            return;
        }
        if (gameState == GameState::WON) {
            res.status = 200;
            res.set_content("you win!", "text/plain");
            return;
        }


#ifdef DATABASE
        // after first analyzer move all others also by analyzer
        if (analyzer) {
            // could not find the move so make a random move
            // const std::string randomMove = ChessPeepo::getRandomInputMove(chessInterface);
            // std::cout << "the db makes the random move: " << randomMove << std::endl;
            // game_state = chessInterface.handleInput(randomMove).value();
            ChessAnalyzer chessAna(chessInterface.getChessBoard());
            const auto bestMoves = chessAna.getBestEvalMoves(1);
            const std::string bestMove = bestMoves.front().second;
            std::cout << "analyzer making the move: " << bestMove << std::endl;
            game_state = chessInterface.handleInput(bestMove).value();
            analyzer = true;

            // return current board position
            const auto board = chessInterface.getChessBoard().getGameHistory().back();
            res.status = 200;
            res.set_content(board, "text/plain");
            return;
        }

        // get oponents move id
        if (auto whiteMoveId = chessDB.getMoveIdOpt(gameDepth, fromMoveId, ChessLinkedListMoves::createData(toTile, false))) {
            fromMoveId = *whiteMoveId;

            // increment after getting the id
            ++gameDepth;

            // get move from oponent and get best next move from db
            auto nextMoves = chessDB.getNextMoves(gameDepth, fromMoveId);

            // transform to pointer vector
            std::vector<MoveCompressed *> nextMovePtrs;
            nextMovePtrs.reserve(nextMoves.size());
            std::ranges::transform(nextMoves, std::back_inserter(nextMovePtrs), [](auto &move) { return &move.second; });

            // get the move string
            auto bestMove = ChessPeepo::getMostPlayedMove(nextMovePtrs);
            std::string dbMove = ChessLinkedListMoves::getMoveFromData(bestMove->data);
            std::cout << "the db makes the move: " << dbMove << std::endl;

            // make the move
            game_state = chessInterface.handleInput(dbMove).value();

            // increment turn
            fromMoveId = *chessDB.getMoveIdOpt(gameDepth, fromMoveId, bestMove->data);
            ++gameDepth;
        } else {
            // could not find the move so make a random move
            // const std::string randomMove = ChessPeepo::getRandomInputMove(chessInterface);
            // std::cout << "the db makes the random move: " << randomMove << std::endl;
            // game_state = chessInterface.handleInput(randomMove).value();
            ChessAnalyzer chessAna(chessInterface.getChessBoard());
            const auto bestMoves = chessAna.getBestEvalMoves(1);
            const std::string bestMove = bestMoves.front().second;
            std::cout << "analyzer making the move: " << bestMove << std::endl;
            game_state = chessInterface.handleInput(bestMove).value();
            analyzer = true;
        }
#else
        ChessAnalyzer chessAna(chessInterface.getChessBoard());
        const auto bestMoves = chessAna.getBestEvalMoves(1);
        const std::string bestMove = bestMoves.front().second;
        std::cout << "analyzer making the move: " << bestMove << std::endl;
        game_state = chessInterface.handleInput(bestMove).value();
#endif

        // return current board position
        const auto board = chessInterface.getChessBoard().getGameHistory().back();
        res.status = 200;
        res.set_content(board, "text/plain");
    });

    std::cout << "starting server" << std::endl;
    const char *p = std::getenv("PORT");
    int port = p ? std::stoi(p) : 8080;
    if (!svr.listen("0.0.0.0", port)) {
        std::cerr << "Error: not listening to server" << std::endl;
    } else {
        std::cout << "server listening on port: " << port << std::endl;
    }
}
