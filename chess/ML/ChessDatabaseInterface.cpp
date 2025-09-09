#include "ChessDatabaseInterface.h"
#include "ChessLinkedListMoves.h"
#include "pqxx/prepared_statement.hxx"
#include "pqxx/zview.hxx"
#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>

ChessDatabaseInterface::ChessDatabaseInterface(const std::string &dbName)
    : connection("dbname=" + dbName + " user=finnp password=Mcstinki+2314") {
    if (connection.is_open()) {
        std::cout << "Connected to: " << dbName << std::endl;
    } else {
        std::cout << "Could not open database: " << dbName << std::endl;
    }

    createPreppareds(200);
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
    pqxx::work worker(connection);
    for (const auto &[fromId, moveData] : move) {
        pqxx::params par{moveData->wins, moveData->loses, moveData->draws, fromId};
        // executePreppedSQL({prepUpdates.at(table)}, par);
        pqxx::result result = worker.exec(pqxx::prepped(prepUpdates.at(table)), par);
    }
    worker.commit();
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

    const std::string sql =
        "SELECT COALESCE ((SELECT mt.id FROM " + joinMoveLinker(table, nextMoveTable) + " WHERE lt.chess_move_id = $1 AND mt.movedata = $2 LIMIT 1), -1) AS id;";
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

    return queryMovesToMap({prepSelectIds.at(nextMoveTable)}, {moveId});
}

int ChessDatabaseInterface::getMaxIdTable(const table_pair &table) {
    const std::string sql = "SELECT COALESCE((SELECT id FROM " + getChessMoveTable(table) + " ORDER BY id DESC LIMIT 1), 1) as first_id";
    return queryMoveId(sql, {});
}

void ChessDatabaseInterface::pushMovesToDB(const ChessLinkedListMoves &llMoves) {
    if (!llMoves.getMoveHead()) return; // is empty

    std::cout << "started pushing moves to db" << std::endl;

    // const auto start = std::chrono::high_resolution_clock::now();

    // the board with no moves on it
    MoveCompressed *headMove = llMoves.getMoveHead();
    static constexpr table_pair baseTable = {false, 0};
    static constexpr int baseId = 1;

    std::queue<table_move_ptr> moveQueue;
    moveQueue.emplace(baseId, headMove);

    table_pair connectTable(baseTable);
    table_pair nextMoveTable(baseTable);
    ++nextMoveTable;

    // auto startTable = std::chrono::high_resolution_clock::now();

    // check to either insert or update existing move
    TabelUpdateData tableUpdateData;

    MoveCompressed *lastDepthElement = headMove;
    // std::chrono::duration<double> durationSelectIds; 
    // std::chrono::duration<double> durationFillTable; 
    while (!moveQueue.empty()) {
        auto [currentId, currentMove] = moveQueue.front();
        moveQueue.pop();

        // get db nexts
        const auto dbNextMovesMap = getNextMovesDataMap(connectTable, currentId);
        // auto endSelectIds = std::chrono::high_resolution_clock::now();
        // durationSelectIds += endSelectIds - startTable;

        fillTableData({currentId, currentMove}, dbNextMovesMap, tableUpdateData);
        // auto endFillingTable = std::chrono::high_resolution_clock::now();
        // durationFillTable += endFillingTable - endSelectIds;

        // reached last element for this move depth
        if (lastDepthElement == currentMove) {
            // std::cout << "total duration of selecting Ids: " << durationSelectIds.count() << std::endl;
            // std::cout << "total duration of filling tables: " << durationFillTable.count() << std::endl;
            // auto endTable = std::chrono::high_resolution_clock::now();
            // std::chrono::duration<double> durationTable = endTable - startTable;
            // std::cout << "done filling current depth after: " << durationTable.count() << " seconds\n";

            updateTable(nextMoveTable, connectTable, moveQueue, tableUpdateData);

            // run analyze on changed tables
            // analyzeTable(getChessMoveTable(nextMoveTable));
            // analyzeTable(getChessLinkerTable(connectTable));

            // new last element to start writign in this depth
            if (moveQueue.empty()) break;
            lastDepthElement = moveQueue.back().second;

            // reset timer
            // startTable = std::chrono::high_resolution_clock::now();
        }
    }

    // const auto end = std::chrono::high_resolution_clock::now();
    // const std::chrono::duration<double> duration = end - start;
    //
    // std::cout << "pushing moves to DB took: " << duration.count() << " seconds\n";
}

void ChessDatabaseInterface::createPreppareds(int depth) {
    table_pair connectTable{false, 0};
    table_pair movesTable(connectTable);
    ++movesTable;

    for (int i = 0; i < depth * 2; i++) {
        const std::string updateSql = "UPDATE " + getChessMoveTable(movesTable) + " SET wins=wins+$1, loses=loses+$2, draws=draws+$3 WHERE id=$4";
        const std::string prepUpdate = "update_statement " + getChessMoveTable(movesTable);
        connection.prepare(prepUpdate, updateSql);
        prepUpdates.emplace(movesTable, prepUpdate);

        const std::string selectIdsSql = "SELECT mt.id, movedata FROM " + joinMoveLinker(connectTable, movesTable) + " WHERE lt.chess_move_id = $1";
        const std::string prepSelect = "select_statement_id " + getChessMoveTable(movesTable);
        connection.prepare(prepSelect, selectIdsSql);
        prepSelectIds.emplace(movesTable, prepSelect);

        ++movesTable;
        ++connectTable;
    }
}

pqxx::result ChessDatabaseInterface::executeSQL(const pqxx::zview sql, const pqxx::params &pars) {
    pqxx::work worker(connection);
    pqxx::result result = worker.exec(sql, pars);
    worker.commit();
    return result;
}

MoveCompressed ChessDatabaseInterface::queryMove(const pqxx::zview sql, const pqxx::params &pars) {
    pqxx::work worker(connection);
    const move_tuple results = worker.query1<pqxx::zview, uint64_t, uint64_t, uint64_t>(sql, pars);
    worker.commit();
    const DataBits bits(getBitsFromDB(std::get<0>(results).substr(2)));
    return MoveCompressed(bits, std::get<1>(results), std::get<2>(results), std::get<3>(results));
}

int ChessDatabaseInterface::queryMoveId(const pqxx::zview sql, const pqxx::params &pars) {
    pqxx::work worker(connection);
    const auto results = worker.query1<int>(sql, pars);
    worker.commit();
    return std::get<0>(results);
}

std::vector<table_move> ChessDatabaseInterface::queryMovesToVec(const pqxx::zview sql, const pqxx::params &pars) {
    pqxx::work worker(connection);
    std::vector<table_move> moves;

    for (auto [id, moveData, wins, loses, draws] : worker.query<int, std::string_view, uint64_t, uint64_t, uint64_t>(sql, pars)) {
        const DataBits bits(getBitsFromDB(std::string_view(moveData).substr(2)));
        moves.emplace_back(id, MoveCompressed{bits, wins, loses, draws});
    }

    worker.commit();
    return moves;
}

ChessDatabaseInterface::nexts_ids_map ChessDatabaseInterface::queryMovesToMap(const pqxx::prepped prepSql, const pqxx::params &pars) {
// ChessDatabaseInterface::nexts_ids_map ChessDatabaseInterface::queryMovesToMap(const pqxx::zview prepSql, const pqxx::params &pars) {
    pqxx::work worker(connection);
    nexts_ids_map moves;

    for (auto [id, moveData] : worker.query<int, std::string_view>(prepSql, pars)) {
        const DataBits bits(getBitsFromDB(moveData.substr(2)));
        moves.emplace(bits, id);
    }

    worker.commit();
    return moves;
}

void ChessDatabaseInterface::analyze() {
    pqxx::work worker(connection);
    worker.exec("ANALYZE");
    worker.commit();
}

void ChessDatabaseInterface::analyzeTable(const std::string &tableName) {
    pqxx::work worker(connection);
    worker.exec("ANALYZE " + tableName);
    worker.commit();
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
    // std::cout << "started writing to table: " << getChessMoveTable(nextMoveTable) << std::endl;
    // const auto startTable = std::chrono::high_resolution_clock::now();

    // insert new moves first
    // as all inserts all happen in the same table. I can take the current max and deduce the new ids from that
    const int maxIdTable = getMaxIdTable(nextMoveTable);
    const int insertMovesSize = updateData.insertMovesDatas.size();
    insertMoves(nextMoveTable, updateData.insertMovesDatas);

    // std::cout << "done inserting moves after: "
    //           << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTable).count() << std::endl;

    std::vector<int> insertMoveIds;
    insertMoveIds.resize(insertMovesSize);
    std::iota(insertMoveIds.begin(), insertMoveIds.end(), maxIdTable);

    // connect the new moves
    connectMoves(connectTable, updateData.insertFromMoveIds, insertMoveIds);

    // std::cout << "done connecting moves after: "
    //           << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTable).count() << std::endl;

    // updating move data
    updateMoves(nextMoveTable, updateData.updateMovesDatas);

    // std::cout << "done updating moves after: "
    //           << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTable).count() << std::endl;

    // add them to the queue
    for (size_t i = 0; i < insertMoveIds.size(); i++) {
        moveQueue.emplace(insertMoveIds.at(i), updateData.insertMovesDatas.at(i));
    }
    for (auto &updateMoves : updateData.updateMovesDatas) {
        moveQueue.emplace(updateMoves);
    }

    // setup for next table
    ++connectTable;
    ++nextMoveTable;
    updateData.insertMovesDatas.clear();
    updateData.insertFromMoveIds.clear();
    updateData.updateMovesDatas.clear();

    // const auto endTable = std::chrono::high_resolution_clock::now();
    // const std::chrono::duration<double> durationTable = endTable - startTable;
    //
    // std::cout << "done writing to current table in: " << durationTable.count() << " seconds\n" << std::endl;
}
