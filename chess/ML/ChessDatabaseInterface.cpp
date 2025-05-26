#include "ChessDatabaseInterface.h"
#include "ChessData.h"
#include "ChessLinkedListMoves.h"
#include "pqxx/internal/statement_parameters.hxx"
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <sys/types.h>

ChessDatabaseInterface::ChessDatabaseInterface(const std::string &dbName)
    : connection("dbname=" + dbName + " user=finnp password=Mcstinki+2314") {
    if (connection.is_open()) {
        std::cout << "Connected to: " << dbName << std::endl;
    } else {
        std::cout << "Could not open database: " << dbName << std::endl;
    }
}

// Did 500 for each color both move and linker
void ChessDatabaseInterface::createTable(const table_pair &table) {
    // might just create maximum ammount of rounds possible which would be 5898 moves by the 50 moves rule*/
    const std::string sql =
        std::format("CREATE TABLE IF NOT EXISTS {} (id SERIAL PRIMARY KEY, moveData CHAR(2), wins BIGINT, loses BIGINT, draws BIGINT);",
                    getChessMoveTable(table));
    if (executeSQL(sql)) {
        std::cout << "Created Table" << std::endl;
    }
}

void ChessDatabaseInterface::createConnectTable(const table_pair &table) {
    const std::string sql = std::format("CREATE TABLE IF NOT EXISTS {} (id SERIAL PRIMARY KEY, chess_move_id INT REFERENCES "
                                        "{}(id), next_id INT REFERENCES {}(id));",
                                        getChessLinkerTable(table), getChessMoveTable(table), getChessLinkerTable(table));
    if (executeSQL(sql)) {
        std::cout << "Created Table" << std::endl;
    }
}

/*void ChessDatabaseInterface::addMove(const table_pair &table, const MoveCompressed &move) {*/
/*    const std::string sql = std::format("INSERT INTO {} (movedata, wins, loses, draws) VALUES ({}, {}, {}, {}) ON CONFLICT ",*/
/*                                              getChessMoveTable(table), move.data.to_string(), move.wins, move.loses, move.draws);*/
/*}*/

int ChessDatabaseInterface::createMove(const table_pair &table, const MoveCompressed &move) {
    pqxx::work worker(connection);

    const std::string sql =
        "INSERT INTO " + getChessMoveTable(table) + " (movedata, wins, loses, draws) VALUES ($1, $2, $3, $4) RETURNING id;";

    // TODO: bruh took so long but can't find how to just pass to a bytea type from the bitset. So this gotta work for now
    auto value = static_cast<uint16_t>(move.data.to_ulong());
    pqxx::params par = pqxx::params{value, move.wins, move.loses, move.draws};
    pqxx::result result = worker.exec(sql, par);

    worker.commit();

    std::cout << "pushed move to table: " << getChessMoveTable(table) << std::endl;

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

void ChessDatabaseInterface::connectMove(const table_pair &table, int sourceId, int targetId) {
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

// recursive foreach of all the moves
int ChessDatabaseInterface::pushMovesToDB(MoveCompressed *head, ChessDatabaseInterface::table_pair table) {

    /**/
    /*MoveCompressed *currentMove;*/
    /*while (currentMove) {*/
    /*    currentMove->nexts*/
    /*}*/
    /**/
    /**/
    /**/
    if (!head) return -1; // reached last move

    const int newId = createMove(table, *head);

    for (const auto &nextMove : head->nexts) {
        if (!table.second) table.first++; // depth only increase after black move
        table.second = !table.second;
        const int nextNewId = pushMovesToDB(nextMove.get(), table);
        connectMove(table, newId, nextNewId);
    }
    return newId;
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
