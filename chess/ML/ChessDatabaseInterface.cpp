#include "ChessDatabaseInterface.h"
#include "ChessLinkedListMoves.h"
#include "pqxx/internal/concat.hxx"
#include "pqxx/internal/statement_parameters.hxx"
#include <cstdint>
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
    auto dataLong = move.data.to_ulong();
    bytes[1] = static_cast<std::byte>(dataLong);
    bytes[0] = static_cast<std::byte>(dataLong >> 8);
    auto sqlBytes = pqxx::binary_cast(bytes, sizeof(bytes));
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

MoveCompressed ChessDatabaseInterface::getMove(const table_pair &table, const int moveId) {
    const std::string sql = "SELECT moveData, wins, loses, draws FROM " + getChessMoveTable(table) + " WHERE id = $1";
    pqxx::params pars(moveId);

    pqxx::work worker(connection);
    auto [moveData, wins, loses, draws] = worker.query1<std::string_view, uint64_t, uint64_t, uint64_t>(sql, pars);
    worker.commit();

    DataBits bits(getBitsFromDB(std::string_view(moveData).substr(2)));
    /*std::string hexMoveData = moveData.substr(2);*/
    /**/
    /*unsigned int intMoveData;*/
    /*std::stringstream ss;*/
    /*ss << std::hex << hexMoveData;*/
    /*ss >> intMoveData;*/

    return MoveCompressed(bits, wins, loses, draws);
}

void ChessDatabaseInterface::connectMove(const table_pair &table, const int sourceId, const int targetId) {
    const std::string sql = "INSERT INTO " + getChessLinkerTable(table) + " (chess_move_id, next_id) VALUES ($1, $2)";
    pqxx::params par = {sourceId, targetId};

    executeSQL(sql, par);
}

// unused
void ChessDatabaseInterface::connectMoves(const table_pair &table, int sourceId, const std::vector<int> &targetIds) {
    pqxx::work worker(connection);
    for (const int targetId : targetIds) {
        worker.exec(std::format("INSERT INTO {} (chess_move_id, next_id) VALUES ({}, {})", getChessLinkerTable(table), sourceId, targetId));
    }
    worker.commit();
}

// TODO:
std::vector<int> ChessDatabaseInterface::getNextMoveIds(int moveId) {
    pqxx::work worker(connection);
    std::vector<int> results;
    const std::string sql = std::format("SELECT next_id FROM chess_moves_linker WHERE chess_move_id = {}", moveId);
    for (const auto &[id] : worker.query<int>(sql)) {
        results.push_back(id);
    }
    worker.commit();
    return results;
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

        if (currentTable.second) currentTable.first++; // depth only increase after white moves
        currentTable.second = !currentTable.second;

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
    pqxx::work c(connection);
    pqxx::result result;
    try {
        result = c.exec(sql, pars);
    } catch (const pqxx::sql_error &e) {
        std::cerr << "SQL error: " << e.what() << std::endl;
        std::cerr << "SQL state: " << e.sqlstate() << std::endl;
        c.commit();
        return std::nullopt;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        c.commit();
        return std::nullopt;
    }
    c.commit();
    return result;
}

DataBits ChessDatabaseInterface::getBitsFromDB(std::string_view bytes) {
    // TODO: can not be the best way to do that. Propably some way in the library for a better solution
    unsigned int intMoveData;
    std::stringstream ss;
    ss << std::hex << bytes;
    ss >> intMoveData;

    return {intMoveData};
}
