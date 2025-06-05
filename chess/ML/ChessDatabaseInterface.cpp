#include "ChessDatabaseInterface.h"
#include "ChessLinkedListMoves.h"
#include "pqxx/internal/concat.hxx"
#include "pqxx/internal/statement_parameters.hxx"
#include <cstdint>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
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

int ChessDatabaseInterface::createMove(const table_pair &table, const MoveCompressed &move) {
    pqxx::work worker(connection);

    const std::string sql =
        "INSERT INTO " + getChessMoveTable(table) + " (movedata, wins, loses, draws) VALUES ($1, $2, $3, $4) RETURNING id;";

    std::byte bytes[2];
    auto dataLong = move.data.to_ulong();
    bytes[1] = std::byte(dataLong);
    bytes[0] = std::byte(dataLong >> 8);
    auto sqlBytes = pqxx::binary_cast(bytes, sizeof(bytes));
    pqxx::params par = pqxx::params{sqlBytes, move.wins, move.loses, move.draws};

    pqxx::result result = worker.exec(sql, par);
    worker.commit();

    /*std::cout << "pushed move to table: " << getChessMoveTable(table) << std::endl;*/

    // get first value which is the id of the created move
    return (*result.begin())[0].as<int>();
}

void ChessDatabaseInterface::updateMove(const table_pair &table, int moveId, int newWins, int newLoses, int newDraws) {
    const std::string sql = std::format("UPDATE {} SET wins = {}, loses = {}, draws = {} WHERE id = {}", getChessMoveTable(table), newWins,
                                        newLoses, newDraws, moveId);
    executeSQL(sql);
}

MoveCompressed ChessDatabaseInterface::getMove(const table_pair &table, int moveId) {
    pqxx::work W(connection);
    auto [moveData, wins, loses, draws] = W.query1<std::string, u_int64_t, uint64_t, uint64_t>(
        std::format("SELECT * FROM {} WHERE id = {}", getChessMoveTable(table), moveId));
    return MoveCompressed(DataBits(moveData), wins, loses, draws);
    W.commit();
}

void ChessDatabaseInterface::connectMove(const table_pair &table, const int sourceId, const int targetId) {
    pqxx::work worker(connection);

    const std::string sql = "INSERT INTO " + getChessLinkerTable(table) + " (chess_move_id, next_id) VALUES ($1, $2)";
    pqxx::params par = {sourceId, targetId};

    worker.exec(sql, par);
    worker.commit();

    std::cout << "create connection between: " << sourceId << " and: " << targetId << std::endl;
}

void ChessDatabaseInterface::connectMoves(const table_pair &table, int sourceId, const std::vector<int> &targetIds) {
    pqxx::work worker(connection);
    for (const int targetId : targetIds) {
        worker.exec(std::format("INSERT INTO {} (chess_move_id, next_id) VALUES ({}, {})", getChessLinkerTable(table), sourceId, targetId));
    }
    worker.commit();
}

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

void ChessDatabaseInterface::pushMovesToDB(const MoveCompressed *head, table_pair table) {
    if (!head) return;

    const auto start = std::chrono::high_resolution_clock::now();

    std::stack<std::tuple<const MoveCompressed *, table_pair, int>> moveStack;

    int connectFromId = createMove(table, *head);
    moveStack.emplace(head, table, connectFromId);

    while (!moveStack.empty()) {
        auto [currentMove, currentTable, currentId] = moveStack.top();
        moveStack.pop();

        const table_pair connectTable(currentTable);

        if (currentTable.second) currentTable.first++; // depth only increase after white moves
        currentTable.second = !currentTable.second;

        for (const auto &nextMove : currentMove->nexts) {
            const int connectToId = createMove(currentTable, *nextMove);
            moveStack.emplace(nextMove.get(), currentTable, connectToId);

            connectMove(connectTable, currentId, connectToId);
        }
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> duration = end - start;

    std::cout << "pushing move to DB took: " << duration.count() << " seconds\n";
}

bool ChessDatabaseInterface::executeSQL(const std::string &sql) {
    try {
        pqxx::work c(connection);
        c.exec(sql);
        c.commit();
    } catch (const pqxx::sql_error &e) {
        std::cerr << "SQL error: " << e.what() << std::endl;
        std::cerr << "SQL state: " << e.sqlstate() << std::endl;
        return false;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
    return true;
}
