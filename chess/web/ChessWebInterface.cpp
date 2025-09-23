#include "ChessWebInterface.h"
#include "ChessInterface.h"
#include "httplib.h"
// #include "nlohmann/json.hpp"

using namespace httplib;
// using nlohmann::json;

ChessWebInterface::ChessWebInterface() {
    // for testing create one chessInstance. I guess that makes it for every user the same board. So gotta learn how every user has its own
    // instance
    ChessInterface chessInterface;

    httplib::Server svr;

    auto ret = svr.set_mount_point("/", "./www");
    if (!ret) {
        std::cerr << "the specified mount point does not exist" << std::endl;
    }

    svr.Get("/reset", [&chessInterface](const Request &req, Response &res) {
        chessInterface = ChessInterface();
        std::cout << "reset chess game" << std::endl;
        res.status = 200;
        res.set_content("game reseted", "text/plain");
    });

    svr.Post("/drag-start", [&chessInterface](const Request &req, Response &res) {
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

    svr.Post("/drag-end", [&chessInterface](const Request &req, Response &res) {
        const std::string toTile = req.body;
        std::cout << "move to tile: " << toTile << std::endl;
        const auto gameState = chessInterface.handleMoveInput(toTile);
        if (!gameState) {
            res.status = 400;
            res.set_content("could not make move", "text/plain");
            return;
        }

        // TODO: response should return gameState, maybe changes of pieces on special move. might just do it in js
        res.status = 200;
        res.set_content("game still going", "text/plain");
    });

    std::cout << "starting server" << std::endl;
    if (!svr.listen("localhost", 8080)) {
        std::cerr << "Error: not listening to server" << std::endl;
    }
}
