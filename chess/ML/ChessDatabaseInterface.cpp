#include "ChessDatabaseInterface.h"

#include <format>
#include <iostream>

ChessDatabaseInterface::ChessDatabaseInterface(const std::string& dbName) {
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
    }
}

ChessDatabaseInterface::~ChessDatabaseInterface() {
    sqlite3_close(db);
}

bool ChessDatabaseInterface::executeSQL(const std::string& sql) {
    char* errorMessage = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        std::cerr << "SQL error: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
        return false;
    }
    return true;
}

bool ChessDatabaseInterface::createNode(const MoveCompressed &move) {
    std::string sql = std::format("INSERT INTO Node (data, wins, loses, draws) VALUES ('{}, {}, {}, {}');",
                                  move.data, move.wins, move.loses, move.draws);
    return executeSQL(sql);
}

bool ChessDatabaseInterface::connectNodes(int sourceId, int targetId) {
    std::string sql = "INSERT INTO NodeConnections (source_id, target_id) VALUES (" + std::to_string(sourceId) + ", " + std::to_string(targetId) + ");";
    return executeSQL(sql);
}

void ChessDatabaseInterface::printNodes() const {
    const char* sql = "SELECT * FROM Node;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char* data = sqlite3_column_text(stmt, 1);
            unsigned long long win = sqlite3_column_int64(stmt, 2);
            unsigned long long lose = sqlite3_column_int64(stmt, 3);
            unsigned long long draw = sqlite3_column_int64(stmt, 4);

            std::cout << "Node ID: " << id
                      << ", Data: " << (char*)data
                      << ", Wins: " << win
                      << ", Losses: " << lose
                      << ", Draws: " << draw << std::endl;
        }
    }
    sqlite3_finalize(stmt);
}

std::vector<std::unique_ptr<MoveCompressed>> ChessDatabaseInterface::getMoves() const{
    const char* sqlSelect = "SELECT * FROM";
}