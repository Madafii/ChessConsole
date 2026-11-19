#include "ChessWebInterface.h"
#include "ChessAnalyzer.h"
#include "ChessBoard.h"
#include "ChessInterface.h"
#ifdef DATABASE
#include "ChessDatabaseInterface.h"
#include "ChessPeepo.h"
#endif
#include "httplib.h"
#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {
bool isSquare(std::string_view square) {
    return square.size() == 2 && square[0] >= 'a' && square[0] <= 'h' && square[1] >= '1' && square[1] <= '8';
}

std::optional<GameState> applyMove(ChessInterface &game, std::string_view move, bool requirePromotion) {
    if ((move.size() != 5 && move.size() != 7) || move[2] != ':' ||
        !isSquare(move.substr(0, 2)) || !isSquare(move.substr(3, 2))) {
        return std::nullopt;
    }

    const auto &from = game.getChessBoard().getTileAt(move.substr(0, 2));
    const bool promotes = from.hasPiece(ChessPieceType::PAWN) && (move[4] == '1' || move[4] == '8');
    if (move.size() == 7 && (!promotes || move[5] != '=' || std::string_view("QRBN").find(move[6]) == std::string_view::npos)) {
        return std::nullopt;
    }
    if (promotes && move.size() == 5) {
        if (requirePromotion) return std::nullopt;
        return game.handleMoveInput(std::string(move) + "=Q");
    }
    return game.handleMoveInput(move);
}

std::optional<GameState> playAnalyzer(ChessInterface &game) {
    auto board = game.getChessBoard();
    ChessAnalyzer analyzer(board);
    auto moves = analyzer.getEvalTree(1);
    const auto *best = analyzer.getBestEvaluatedMove(moves);
    if (!best) return std::nullopt;
    return applyMove(game, best->s_move, false);
}

int serverPort() {
    const char *configured = std::getenv("PORT");
    if (!configured) return 8080;

    const std::string_view value(configured);
    int port = 0;
    const auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), port);
    if (error != std::errc{} || end != value.data() + value.size() || port < 1 || port > 65535) {
        throw std::runtime_error("PORT must be an integer from 1 to 65535");
    }
    return port;
}
} // namespace

using httplib::Request;
using httplib::Response;

void ChessWebInterface::run() {
    auto game = std::make_unique<ChessInterface>();
    auto state = GameState::InProgress;
    std::mutex gameMutex;

#ifdef DATABASE
    std::unique_ptr<ChessDatabaseInterface> database;
    try {
        database = std::make_unique<ChessDatabaseInterface>("chessMoves");
    } catch (const std::exception &error) {
        std::cerr << "Opening database unavailable: " << error.what() << '\n';
    }
    table_pair depth(0, false);
    int previousMove = 1;
    bool useBook = database != nullptr;

    const auto playBook = [&](ChessInterface &next, std::string_view move) -> std::optional<GameState> {
        if (!useBook) return std::nullopt;

        try {
            const auto playerId = database->getMoveIdOpt(depth, previousMove, ChessLinkedListMoves::createData(move, false));
            if (playerId) {
                auto replyDepth = depth;
                ++replyDepth;
                auto replies = database->getNextMoves(replyDepth, *playerId);
                std::vector<MoveCompressed *> moves;
                moves.reserve(replies.size());
                for (auto &reply : replies) moves.push_back(&reply.second);

                if (!moves.empty()) {
                    const auto *best = ChessPeepo::getMostPlayedMove(moves);
                    if (const auto replyId = database->getMoveIdOpt(replyDepth, *playerId, best->data)) {
                        if (auto result = applyMove(next, ChessLinkedListMoves::getMoveFromData(best->data), false)) {
                            previousMove = *replyId;
                            depth = ++replyDepth;
                            return result;
                        }
                    }
                }
            }
        } catch (const std::exception &error) {
            std::cerr << "Opening database lookup failed: " << error.what() << '\n';
        }
        useBook = false;
        return std::nullopt;
    };
#endif

    httplib::Server server;
    const char *configuredRoot = std::getenv("CHESS_WEB_ROOT");
    const auto root = configuredRoot ? std::filesystem::path(configuredRoot)
                                    : std::filesystem::path(std::filesystem::exists("web/www") ? "web/www" : "www");
    if (!server.set_mount_point("/", root.string())) {
        throw std::runtime_error("Cannot serve web assets from " + root.string());
    }

    std::string_view status = "playing";
    std::uint64_t revision = 0;
    const auto isCurrent = [&](const Request &request, Response &response) {
        if (!request.has_header("X-Board-Revision") ||
            request.get_header_value("X-Board-Revision") == std::to_string(revision)) {
            return true;
        }
        response.status = 409;
        response.set_content("The board changed. Please try again.", "text/plain");
        return false;
    };
    const auto sendBoard = [&](Response &response) {
        response.set_header("Cache-Control", "no-store");
        response.set_header("X-Game-Status", std::string(status));
        response.set_header("X-Board-Revision", std::to_string(revision));
        response.set_content(game->getChessBoard().getStringFromBoard(), "text/plain");
    };

    server.Get("/board", [&](const Request &, Response &response) {
        const std::lock_guard lock(gameMutex);
        sendBoard(response);
    });

    const auto reset = [&](const Request &request, Response &response) {
        const std::lock_guard lock(gameMutex);
        game = std::make_unique<ChessInterface>();
        ++revision;
        state = GameState::InProgress;
        status = "playing";
#ifdef DATABASE
        depth = {0, false};
        previousMove = 1;
        useBook = database != nullptr;
#endif
        if (request.method == "GET") {
            response.set_header("Cache-Control", "no-store");
            response.set_content("game reseted", "text/plain");
        } else {
            sendBoard(response);
        }
    };
    server.Get("/reset", reset);
    server.Post("/reset", reset);

    server.Post("/drag-start", [&](const Request &request, Response &response) {
        const std::lock_guard lock(gameMutex);
        if (!isCurrent(request, response)) return;
        if (state != GameState::InProgress) {
            response.status = 409;
            response.set_content("The game has ended.", "text/plain");
            return;
        }
        const auto moves = isSquare(request.body) ? game->getPossibleMovesFromTile(request.body) : std::nullopt;
        if (!moves) {
            response.status = 400;
            response.set_content("Select one of your own pieces.", "text/plain");
            return;
        }

        std::string destinations;
        for (const auto *move : *moves) {
            if (!destinations.empty()) destinations += ' ';
            destinations += move->getPos();
        }
        response.set_content(destinations, "text/plain");
    });

    server.Post("/drag-end", [&](const Request &request, Response &response) {
        const std::lock_guard lock(gameMutex);
        if (!isCurrent(request, response)) return;
        if (state != GameState::InProgress) {
            response.status = 409;
            response.set_content("The game has ended.", "text/plain");
            return;
        }

        auto next = std::make_unique<ChessInterface>(game->getChessBoard());
        auto result = applyMove(*next, request.body, true);
        if (!result) {
            response.status = 400;
            response.set_content("That move is invalid.", "text/plain");
            return;
        }

        const bool playerWon = *result == GameState::Won;
        if (*result == GameState::InProgress) {
#ifdef DATABASE
            result = playBook(*next, request.body);
            if (!result)
#endif
                result = playAnalyzer(*next);
            if (!result) {
                response.status = 500;
                response.set_content("The opponent could not move. Please try again.", "text/plain");
                return;
            }
        }

        game = std::move(next);
        ++revision;
        state = *result;
        switch (state) {
        case GameState::InProgress: status = "playing"; break;
        case GameState::Draw: status = "draw"; break;
        case GameState::Won: status = playerWon ? "white-won" : "black-won"; break;
        }
        if (playerWon && !request.has_header("X-Board-Revision")) {
            response.set_content("you win!", "text/plain");
        } else {
            sendBoard(response);
        }
    });

    const int port = serverPort();
    if (!server.bind_to_port("0.0.0.0", port)) {
        throw std::runtime_error("Cannot bind web server to port " + std::to_string(port));
    }
    std::cout << "Web server listening on port " << port << std::endl;
    if (!server.listen_after_bind()) throw std::runtime_error("Web server stopped unexpectedly");
}
