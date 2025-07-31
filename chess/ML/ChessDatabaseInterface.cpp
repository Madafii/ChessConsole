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
#include <unordered_map>

ChessDatabaseInterface::ChessDatabaseInterface(const std::string &dbName)
    : connection("dbname=" + dbName + " user=finnp password=Mcstinki+2314") {
    if (connection.is_open()) {
        std::cout << "Connected to: " << dbName << std::endl;
    } else {
        std::cout << "Could not open database: " << dbName << std::endl;
    }
}

std::optional<int> ChessDatabaseInterface::insertMove(const table_pair &table, const MoveCompressed &move) {
    const std::string sql =
        "INSERT INTO " + getChessMoveTable(table) + " (movedata, wins, loses, draws) VALUES ($1, $2, $3, $4) RETURNING id;";

    // const auto sqlBytes = getBytesFromDataBits(move.data);
    // std::cout << getDBHexStringFromMoveData(move.data) << std::endl;
    const auto sqlBytes = getDBHexStringFromMoveData(move.data);
    const pqxx::params par(sqlBytes, move.wins, move.loses, move.draws);

    if (const auto result = executeSQL(sql, par)) {
        // get first value which is the id of the created move
        return result->begin()->at(0).as<int>();
    }

    std::cerr << "error: pushing the move " << ChessLinkedListMoves::getBasicInfo(&move);
    return std::nullopt;
}

void ChessDatabaseInterface::insertMoves(const table_pair &table, const std::vector<MoveCompressed *> &moves) {
    pqxx::work worker(connection);
    pqxx::stream_to moveTableStream = pqxx::stream_to::table(worker, {getChessMoveTable(table)}, {"movedata", "wins", "loses", "draws"});

    for (const auto &move : moves) {
        // no fucking idea why this does not work suddenly
        // const auto dbBytes = getBytesFromDataBits(move->data);
        const auto sqlBytes = getDBHexStringFromMoveData(move->data);
        moveTableStream.write_values(sqlBytes, move->wins, move->loses, move->draws);
    }

    moveTableStream.complete();
    worker.commit();
}

// TODO:
void ChessDatabaseInterface::updateMove(const table_pair &table, int moveId, int newWins, int newLoses, int newDraws) {
    const std::string sql = "UPDATE " + getChessMoveTable(table) + " SET wins=wins+$1, loses=loses+$2, draws=draws+$3 WHERE id=$4";
    pqxx::params par{newWins, newLoses, newDraws, moveId};
    executeSQL(sql, par);
}

void ChessDatabaseInterface::updateMoves(const table_pair &table, const std::vector<std::pair<int, MoveCompressed *>> &move) {
    const std::string sql = "UPDATE " + getChessMoveTable(table) + " SET wins=wins+$1, loses=loses+$2, draws=draws+$3 WHERE id=$4";

    for (const auto &[fromId, moveData] : move) {
        pqxx::params par{moveData->wins, moveData->loses, moveData->draws, fromId};
        executeSQL(sql, par);
    }
}

std::optional<MoveCompressed> ChessDatabaseInterface::getMove(const table_pair &table, const int moveId) {
    const std::string sql = selectCompressedMoveFrom + getChessMoveTable(table) + " WHERE id = $1";
    pqxx::params pars(moveId);

    return queryMove(sql, pars);
}

std::optional<MoveCompressed> ChessDatabaseInterface::getMove(const table_pair &table, const int fromMoveId, const DataBits &moveData) {
    table_pair nextMoveTable(table);
    ++nextMoveTable;

    const std::string sql =
        selectCompressedMoveFrom + joinMoveLinker(table, nextMoveTable) + " WHERE lt.chess_move_id = $1 AND mt.movedata = $2";
    pqxx::params pars(fromMoveId, getDBHexStringFromMoveData(moveData));

    return queryMove(sql, pars);
}

std::optional<int> ChessDatabaseInterface::getMoveId(const table_pair &table, int fromMoveId, const DataBits &moveData) {
    table_pair nextMoveTable(table);
    ++nextMoveTable;

    const std::string sql =
        "SELECT mt.id FROM " + joinMoveLinker(table, nextMoveTable) + " WHERE lt.chess_move_id = $1 AND mt.movedata = $2";
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

void ChessDatabaseInterface::connectMoves(const table_pair &table, const std::vector<int> &sourceIds, const std::vector<int> &targetIds) {
    pqxx::work worker(connection);
    pqxx::stream_to connectTableStream = pqxx::stream_to::table(worker, {getChessLinkerTable(table)}, {"chess_move_id", "next_id"});
    const int connectionsSize = sourceIds.size();
    for (int i = 0; i < connectionsSize; i++) {
        connectTableStream.write_values(sourceIds.at(i), targetIds.at(i));
    }

    connectTableStream.complete();
    worker.commit();
}

std::vector<std::pair<int, MoveCompressed>> ChessDatabaseInterface::getNextMoves(const table_pair &table, int moveId) {
    std::vector<MoveCompressed> resultMoves;

    table_pair nextMoveTable(table);
    ++nextMoveTable;

    const std::string sql = selectAllMoveTable + joinMoveLinker(table, nextMoveTable) + " WHERE lt.chess_move_id = $1";
    const pqxx::params par(moveId);

    return queryMoves(sql, par);
}

std::unordered_map<DataBits, int, DataBitsHash> ChessDatabaseInterface::getNextMovesDataMap(const table_pair &table, int moveId) {
    const auto dbNextMoves = getNextMoves(table, moveId);
    std::unordered_map<DataBits, int, DataBitsHash> dbNextMovesMap;
    for (const auto &dbNextMove : dbNextMoves) {
        dbNextMovesMap[dbNextMove.second.data] = dbNextMove.first;
        // dbNextMovesMap.insert(dbNextMove.second.data, dbNextMove.second);
    }
    return dbNextMovesMap;
}

int ChessDatabaseInterface::getMaxIdTable(const table_pair &table) {
    const std::string sql = "SELECT COALESCE((SELECT id FROM " + getChessMoveTable(table) + " ORDER BY id DESC LIMIT 1), 1) as first_id";
    return *queryMoveId(sql, {});
}

// starts from the head of the linked list (so set to root if wanted)
void ChessDatabaseInterface::pushMovesToDBOld(const ChessLinkedListMoves &llMoves, table_pair table) {
    if (!llMoves.getMoveHead()) return;

    std::cout << "started pushing moves to db" << std::endl;

    const auto start = std::chrono::high_resolution_clock::now();

    std::stack<std::tuple<const MoveCompressed *, table_pair, int>> moveStack;

    if (auto connectFromId = insertMove(table, *llMoves.getMoveHead())) {
        moveStack.emplace(llMoves.getMoveHead(), table, *connectFromId);
    }

    while (!moveStack.empty()) {
        auto [currentMove, currentTable, currentId] = moveStack.top();
        moveStack.pop();

        const table_pair connectTable(currentTable);

        ++currentTable;

        for (const auto &nextMove : currentMove->nexts) {
            if (auto connectToId = insertMove(currentTable, *nextMove)) {
                moveStack.emplace(nextMove.get(), currentTable, *connectToId);

                connectMove(connectTable, currentId, *connectToId);
            }
        }
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> duration = end - start;

    std::cout << "pushing move to DB took: " << duration.count() << " seconds\n";
}

void ChessDatabaseInterface::pushMovesToDB(const ChessLinkedListMoves &llMoves) {
    if (!llMoves.getMoveHead()) return; // is empty

    std::cout << "started pushing moves to db" << std::endl;

    const auto start = std::chrono::high_resolution_clock::now();

    // the board with no moves on it
    MoveCompressed *headMove = llMoves.getMoveHead();
    constexpr table_pair baseTable = {false, 0};
    constexpr int baseId = 1;
    // create base board in case it does not exist
    if (getMove(baseTable, baseId) == std::nullopt) {
        insertMove(baseTable, *headMove);
    }

    std::queue<std::pair<int, MoveCompressed *>> moveQueue;
    moveQueue.emplace(baseId, headMove);

    table_pair connectTable(baseTable);
    table_pair nextMoveTable(baseTable);
    ++nextMoveTable;

    // check to either insert or update existing move
    TabelUpdateData tableUpdateData;

    MoveCompressed *lastDepthElement = headMove;
    while (!moveQueue.empty()) {
        auto [currentId, currentMove] = moveQueue.front();
        moveQueue.pop();

        // get db nexts
        const auto dbNextMovesMap = getNextMovesDataMap(connectTable, currentId);
        fillTableData({currentId, currentMove}, dbNextMovesMap, tableUpdateData);

        // reached last element for this move depth
        if (lastDepthElement == currentMove) {
            updateTable(nextMoveTable, connectTable, moveQueue, tableUpdateData);
            // new last element to start writign in this depth
            if (moveQueue.empty()) break;
            lastDepthElement = moveQueue.back().second;
        }
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> duration = end - start;

    std::cout << "pushing moves to DB took: " << duration.count() << " seconds\n";
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

std::vector<std::pair<int, MoveCompressed>> ChessDatabaseInterface::queryMoves(const std::string &sql, const pqxx::params &pars) {
    pqxx::work worker(connection);
    std::vector<std::pair<int, MoveCompressed>> moves;

    try {
        for (auto [id, moveData, wins, loses, draws] : worker.query<int, std::string_view, uint64_t, uint64_t, uint64_t>(sql, pars)) {
            const DataBits bits(getBitsFromDB(std::string_view(moveData).substr(2)));
            moves.emplace_back(id, MoveCompressed{bits, wins, loses, draws});
        }
    } catch (std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
    }

    worker.commit();
    return moves;
}

void ChessDatabaseInterface::updateTable(table_pair &nextMoveTable, table_pair &connectTable,
                                         std::queue<std::pair<int, MoveCompressed *>> moveQueue, TabelUpdateData &updateData) {

    // std::cout << "started writing to table" << getChessMoveTable(nextMoveTable) << std::endl;
    // const auto startTable = std::chrono::high_resolution_clock::now();

    // insert new moves first
    // as all inserts all happen in the same table. I can take the current max and deduce the new ids from that
    const int maxIdTable = getMaxIdTable(nextMoveTable);
    const int insertMovesSize = updateData.insertMovesDatas.size();
    insertMoves(nextMoveTable, updateData.insertMovesDatas);

    std::vector<int> insertMoveIds;
    insertMoveIds.resize(insertMovesSize);
    std::iota(insertMoveIds.begin(), insertMoveIds.end(), maxIdTable);

    // connect the new moves
    connectMoves(connectTable, updateData.insertFromMoveIds, insertMoveIds);

    // updating move data
    updateMoves(nextMoveTable, updateData.updateMovesDatas);

    // add them to the queue
    for (size_t i = 0; i < insertMoveIds.size(); i++) {
        moveQueue.emplace(insertMoveIds.at(i), updateData.insertMovesDatas.at(i));
    }
    moveQueue.push_range(updateData.updateMovesDatas);

    // setup for next table
    ++connectTable;
    ++nextMoveTable;
    updateData.insertMovesDatas.clear();
    updateData.insertFromMoveIds.clear();
    updateData.updateMovesDatas.clear();

    // const auto endTable = std::chrono::high_resolution_clock::now();
    // const std::chrono::duration<double> durationTable = endTable - startTable;
    //
    // std::cout << "done writing to current table in: " << durationTable.count() << " seconds\n";
}

void ChessDatabaseInterface::fillTableData(const std::pair<int, MoveCompressed *> currentMoveData,
                                           const std::unordered_map<DataBits, int, DataBitsHash> &dbNextMovesMap,
                                           TabelUpdateData &updateData) {

    for (const auto &nextMove : currentMoveData.second->nexts) {
        if (dbNextMovesMap.contains(nextMove->data)) {
            updateData.updateMovesDatas.emplace_back(dbNextMovesMap.at(nextMove->data), nextMove.get());
            // updateFromMoveIds.emplace_back(currentId);
        } else {
            updateData.insertMovesDatas.push_back(nextMove.get());
            updateData.insertFromMoveIds.push_back(currentMoveData.first);
        }
    }
}
