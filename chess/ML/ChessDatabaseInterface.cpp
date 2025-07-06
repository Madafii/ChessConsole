#include "ChessDatabaseInterface.h"
#include "ChessLinkedListMoves.h"
#include "pqxx/internal/concat.hxx"
#include "pqxx/internal/statement_parameters.hxx"
#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <tuple>

ChessDatabaseInterface::ChessDatabaseInterface(const std::string &dbName)
    : connection("dbname=" + dbName + " user=finnp password=Mcstinki+2314") {
    if (connection.is_open()) {
        std::cout << "Connected to: " << dbName << std::endl;
    } else {
        std::cout << "Could not open database: " << dbName << std::endl;
    }
}

std::optional<int> ChessDatabaseInterface::createMove(const table_pair &table, const MoveCompressed &move) {
    const std::string sql =
        "INSERT INTO " + getChessMoveTable(table) + " (movedata, wins, loses, draws) VALUES ($1, $2, $3, $4) RETURNING id;";

    std::byte bytes[2];
    const auto dataLong = move.data.to_ulong();
    bytes[1] = static_cast<std::byte>(dataLong);
    bytes[0] = static_cast<std::byte>(dataLong >> 8);
    const auto sqlBytes = pqxx::binary_cast(bytes, sizeof(bytes));
    const pqxx::params par(sqlBytes, move.wins, move.loses, move.draws);

    if (const auto result = executeSQL(sql, par)) {
        // get first value which is the id of the created move
        return result->begin()->at(0).as<int>();
    }

    std::cerr << "error: pushing the move " << ChessLinkedListMoves::getBasicInfo(&move);
    return std::nullopt;
}

// TODO:
void ChessDatabaseInterface::updateMove(const table_pair &table, int moveId, int newWins, int newLoses, int newDraws) {
    const std::string sql = std::format("UPDATE {} SET wins = {}, loses = {}, draws = {} WHERE id = {}", getChessMoveTable(table), newWins,
                                        newLoses, newDraws, moveId);
    /*executeSQL(sql);*/
}

std::optional<MoveCompressed> ChessDatabaseInterface::getMove(const table_pair &table, const int moveId) {
    const std::string sql = selectCompressedMoveFrom + getChessMoveTable(table) + " WHERE id = $1";
    pqxx::params pars(moveId);

    return queryMove(sql, pars);
}

std::optional<MoveCompressed> ChessDatabaseInterface::getMove(const table_pair &table, const int fromMoveId, const DataBits &moveData) {
    table_pair nextMoveTable(table);
    incrementTable(nextMoveTable);

    const std::string sql = selectCompressedMoveFrom + joinMoveLinker(table, nextMoveTable) + " WHERE lt.chess_move_id = $1 AND mt.movedata = $2";
    pqxx::params pars(fromMoveId, getDBHexStringFromMoveData(moveData));

    return queryMove(sql, pars);
}

std::optional<int> ChessDatabaseInterface::getMoveId(const table_pair &table, int fromMoveId, const DataBits &moveData) {
    table_pair nextMoveTable(table);
    incrementTable(nextMoveTable);

    const std::string sql = "SELECT mt.id FROM " + joinMoveLinker(table, nextMoveTable) + " WHERE lt.chess_move_id = $1 AND mt.movedata = $2";
    pqxx::params pars(fromMoveId, getDBHexStringFromMoveData(moveData));

    return queryMoveId(sql, pars);
}

void ChessDatabaseInterface::connectMove(const table_pair &table, const int sourceId, const int targetId) {
    const std::string sql = "INSERT INTO " + getChessLinkerTable(table) + " (chess_move_id, next_id) VALUES ($1, $2)";
    pqxx::params par(sourceId, targetId);

    executeSQL(sql, par);
}

// unused TODO:
void ChessDatabaseInterface::connectMoves(const table_pair &table, int sourceId, const std::vector<int> &targetIds) {
    pqxx::work worker(connection);
    for (const int targetId : targetIds) {
        worker.exec(std::format("INSERT INTO {} (chess_move_id, next_id) VALUES ({}, {})", getChessLinkerTable(table), sourceId, targetId));
    }
    worker.commit();
}

std::vector<MoveCompressed> ChessDatabaseInterface::getNextMoves(const table_pair &table, int moveId) {
    std::vector<MoveCompressed> resultMoves;

    table_pair nextMoveTable(table);
    incrementTable(nextMoveTable);

    const std::string sql = selectCompressedMoveFrom + joinMoveLinker(table, nextMoveTable) + " WHERE lt.chess_move_id = $1";
    const pqxx::params par(moveId);

    return queryMoves(sql, par);
}

// starts from the head of the linked list (so set to root if wanted)
void ChessDatabaseInterface::pushMovesToDB(const ChessLinkedListMoves &llMoves, table_pair table) {
    if (!llMoves.getMoveHead()) return;

    const auto start = std::chrono::high_resolution_clock::now();

    std::stack<std::tuple<const MoveCompressed *, table_pair, int>> moveStack;

    if (auto connectFromId = createMove(table, *llMoves.getMoveHead())) {
        moveStack.emplace(llMoves.getMoveHead(), table, *connectFromId);
    }

    while (!moveStack.empty()) {
        auto [currentMove, currentTable, currentId] = moveStack.top();
        moveStack.pop();

        const table_pair connectTable(currentTable);

        incrementTable(currentTable);

        for (const auto &nextMove : currentMove->nexts) {
            if (auto connectToId = createMove(currentTable, *nextMove)) {
                moveStack.emplace(nextMove.get(), currentTable, *connectToId);

                connectMove(connectTable, currentId, *connectToId);
            }
        }
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> duration = end - start;

    std::cout << "pushing move to DB took: " << duration.count() << " seconds\n";
}

std::optional<pqxx::result> ChessDatabaseInterface::executeSQL(const std::string &sql, const pqxx::params &pars) {
    pqxx::work worker(connection);
    pqxx::result result;
    try {
        result = worker.exec(sql, pars);
    } catch (const pqxx::sql_error &e) {
        std::cerr << "SQL error: " << e.what() << std::endl;
        std::cerr << "SQL state: " << e.sqlstate() << std::endl;
        worker.commit();
        return std::nullopt;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        worker.commit();
        return std::nullopt;
    }
    worker.commit();
    return result;
}

std::optional<MoveCompressed> ChessDatabaseInterface::queryMove(const std::string &sql, const pqxx::params &pars) {
    pqxx::work worker(connection);
    move_tuple results;
    try {
        results = worker.query1<std::string_view, uint64_t, uint64_t, uint64_t>(sql, pars);
    } catch (const pqxx::unexpected_rows &e) {
        std::cerr << "SQL error: " << e.what() << std::endl;
        worker.commit();
        return std::nullopt;
    } catch (std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
        worker.commit();
        return std::nullopt;
    }
    worker.commit();
    const DataBits bits(getBitsFromDB(std::string_view(std::get<0>(results)).substr(2)));
    return MoveCompressed(bits, std::get<1>(results), std::get<2>(results), std::get<3>(results));
}

std::optional<int> ChessDatabaseInterface::queryMoveId(const std::string &sql, const pqxx::params &pars) {
    pqxx::work worker(connection);
    std::tuple<int> results;
    try {
        results = worker.query1<int>(sql, pars);
    } catch (const pqxx::unexpected_rows &e) {
        std::cerr << "SQL error: " << e.what() << std::endl;
        worker.commit();
        return std::nullopt;
    } catch (std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
        worker.commit();
        return std::nullopt;
    }
    worker.commit();
    return std::get<0>(results);
}

std::vector<MoveCompressed> ChessDatabaseInterface::queryMoves(const std::string &sql, const pqxx::params &pars) {
    pqxx::work worker(connection);
    std::vector<MoveCompressed> moves;

    try {
        for (auto [moveData, wins, loses, draws] : worker.query<std::string_view, uint64_t, uint64_t, uint64_t>(sql, pars)) {
            const DataBits bits(getBitsFromDB(std::string_view(moveData).substr(2)));
            moves.emplace_back(bits, wins, loses, draws);
        }
    } catch (std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
    }

    worker.commit();
    return moves;
}
