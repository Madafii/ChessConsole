#include "ChessDatabaseInterface.h"
#include "ChessLinkedListMoves.h"
#include <cstdint>
#include <iostream>
#include <memory>
#include <tuple>

ChessDatabaseInterface::ChessDatabaseInterface(const std::string &dbName)
    : connection("dbname=" + dbName + " user=finnp password=password") {
    if (connection.is_open()) {
        std::cout << "Connected to: " << dbName << std::endl;
    } else {
        std::cout << "Could not open database: " << dbName << std::endl;
    }
}

int ChessDatabaseInterface::insertMove(const table_pair &table, const MoveCompressed &move) {
    const std::string sql =
        "INSERT INTO " + getChessMoveTable(table) + " (movedata, wins, loses, draws) VALUES ($1, $2, $3, $4) RETURNING id;";

    // why does this not work anymoooore
    // const auto sqlBytes = getBytesFromDataBits(move.data);
    // std::cout << getDBHexStringFromMoveData(move.data) << std::endl;
    const auto sqlBytes = getDBHexStringFromMoveData(move.data);
    const pqxx::params par(sqlBytes, move.wins, move.loses, move.draws);

    const auto result = executeSQL(sql, par);
    // get first value which is the id of the created move
    return result.begin()->at(0).as<int>();
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

void ChessDatabaseInterface::updateMove(const table_pair &table, int moveId, int newWins, int newLoses, int newDraws) {
    const std::string sql = "UPDATE " + getChessMoveTable(table) + " SET wins=wins+$1, loses=loses+$2, draws=draws+$3 WHERE id=$4";
    pqxx::params par{newWins, newLoses, newDraws, moveId};
    executeSQL(sql, par);
}

void ChessDatabaseInterface::updateMoves(const table_pair &table, const std::vector<table_move_ptr> &move) {
    const std::string sql = "UPDATE " + getChessMoveTable(table) + " SET wins=wins+$1, loses=loses+$2, draws=draws+$3 WHERE id=$4";
    connection.prepare("update_statement", sql);

    for (const auto &[fromId, moveData] : move) {
        pqxx::params par{moveData->wins, moveData->loses, moveData->draws, fromId};
        executeSQL(sql, par);
    }
}

MoveCompressed ChessDatabaseInterface::getMove(const table_pair &table, const int moveId) {
    const std::string sql = "SELECT moveData, wins, loses, draws FROM " + getChessMoveTable(table) + " WHERE id = $1";
    pqxx::params pars(moveId);

    return queryMove(sql, pars);
}

MoveCompressed ChessDatabaseInterface::getMove(const table_pair &table, const int fromMoveId, const DataBits &moveData) {
    table_pair nextMoveTable(table);
    ++nextMoveTable;

    const std::string sql = "SELECT moveData, wins, loses, draws FROM " + joinMoveLinker(table, nextMoveTable) +
                            " WHERE lt.chess_move_id = $1 AND mt.movedata = $2";
    pqxx::params pars(fromMoveId, getDBHexStringFromMoveData(moveData));

    return queryMove(sql, pars);
}

int ChessDatabaseInterface::getMoveId(const table_pair &table, int fromMoveId, const DataBits &moveData) {
    table_pair nextMoveTable(table);
    ++nextMoveTable;

    const std::string sql =
        "SELECT mt.id FROM " + joinMoveLinker(table, nextMoveTable) + " WHERE lt.chess_move_id = $1 AND mt.movedata = $2";
    const pqxx::params pars(fromMoveId, getDBHexStringFromMoveData(moveData));

    return queryMoveId(sql, pars);
}

std::optional<int> ChessDatabaseInterface::getMoveIdOpt(const table_pair &table, int fromMoveId, const DataBits &moveData){
    table_pair nextMoveTable(table);
    ++nextMoveTable;

    // TODO: make it return -1 if no value or the id
    const std::string sql =
        "SELECT exists (SELECT 1 FROM " + joinMoveLinker(table, nextMoveTable) + " WHERE lt.chess_move_id = $1 AND mt.movedata = $2 LIMIT 1);";
    const pqxx::params pars(fromMoveId, getDBHexStringFromMoveData(moveData));

    int moveId = queryMoveId(sql, pars);
    if (moveId < 0) {
        return std::nullopt;
    }
    return moveId;
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

std::vector<table_move> ChessDatabaseInterface::getNextMoves(const table_pair &table, int moveId) {
    table_pair nextMoveTable(table);
    ++nextMoveTable;

    const std::string sql =
        "SELECT mt.id, movedata, wins, loses, draws FROM " + joinMoveLinker(table, nextMoveTable) + " WHERE lt.chess_move_id = $1";
    const pqxx::params par(moveId);

    return queryMovesToVec(sql, par);
}

ChessDatabaseInterface::nexts_ids_map ChessDatabaseInterface::getNextMovesDataMap(const table_pair &table, int moveId) {
    table_pair nextMoveTable(table);
    ++nextMoveTable;

    const std::string sql = "SELECT mt.id, movedata FROM " + joinMoveLinker(table, nextMoveTable) + " WHERE lt.chess_move_id = $1";
    const pqxx::params par(moveId);

    return queryMovesToMap(sql, par);
}

int ChessDatabaseInterface::getMaxIdTable(const table_pair &table) {
    const std::string sql = "SELECT COALESCE((SELECT id FROM " + getChessMoveTable(table) + " ORDER BY id DESC LIMIT 1), 1) as first_id";
    return queryMoveId(sql, {});
}

void ChessDatabaseInterface::pushMovesToDB(const ChessLinkedListMoves &llMoves) {
    if (!llMoves.getMoveHead()) return; // is empty

    std::cout << "started pushing moves to db" << std::endl;

    const auto start = std::chrono::high_resolution_clock::now();

    // the board with no moves on it
    MoveCompressed *headMove = llMoves.getMoveHead();
    constexpr table_pair baseTable = {false, 0};
    constexpr int baseId = 1;

    std::queue<table_move_ptr> moveQueue;
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

pqxx::result ChessDatabaseInterface::executeSQL(const std::string &sql, const pqxx::params &pars) {
    pqxx::work worker(connection);
    pqxx::result result = worker.exec(sql, pars);
    worker.commit();
    return result;
}

pqxx::result ChessDatabaseInterface::executeSQL(const std::string &sql, const pqxx::params &pars) {
    pqxx::work worker(connection);
    pqxx::result result = worker.exec(sql, pars);
    worker.commit();
    return result;
}

MoveCompressed ChessDatabaseInterface::queryMove(const std::string &sql, const pqxx::params &pars) {
    pqxx::work worker(connection);
    move_tuple results = worker.query1<std::string_view, uint64_t, uint64_t, uint64_t>(sql, pars);
    worker.commit();
    const DataBits bits(getBitsFromDB(std::get<0>(results).substr(2)));
    return MoveCompressed(bits, std::get<1>(results), std::get<2>(results), std::get<3>(results));
}

int ChessDatabaseInterface::queryMoveId(const std::string &sql, const pqxx::params &pars) {
    pqxx::work worker(connection);
    const auto results = worker.query1<int>(sql, pars);
    worker.commit();
    return std::get<0>(results);
}

std::vector<table_move> ChessDatabaseInterface::queryMovesToVec(const std::string &sql, const pqxx::params &pars) {
    pqxx::work worker(connection);
    std::vector<table_move> moves;

    for (auto [id, moveData, wins, loses, draws] : worker.query<int, std::string_view, uint64_t, uint64_t, uint64_t>(sql, pars)) {
        const DataBits bits(getBitsFromDB(std::string_view(moveData).substr(2)));
        moves.emplace_back(id, MoveCompressed{bits, wins, loses, draws});
    }

    worker.commit();
    return moves;
}

ChessDatabaseInterface::nexts_ids_map ChessDatabaseInterface::queryMovesToMap(const std::string &sql, const pqxx::params &pars) {
    pqxx::work worker(connection);
    nexts_ids_map moves;

    for (auto [id, moveData] : worker.query<int, std::string_view>(sql, pars)) {
        const DataBits bits(getBitsFromDB(moveData.substr(2)));
        moves.emplace(bits, id);
    }

    worker.commit();
    return moves;
}

void ChessDatabaseInterface::fillTableData(const table_move_ptr &currentMoveData, const nexts_ids_map &dbNextMovesMap,
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

void ChessDatabaseInterface::updateTable(table_pair &nextMoveTable, table_pair &connectTable,
                                         std::queue<table_move_ptr> &moveQueue, TabelUpdateData &updateData) {
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
    for (auto &updateMoves : updateData.updateMovesDatas) {
        moveQueue.emplace(updateMoves.first, updateMoves.second);
    }
    // TODO: check remember that worked???
    // moveQueue.push_range(updateData.updateMovesDatas);

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
