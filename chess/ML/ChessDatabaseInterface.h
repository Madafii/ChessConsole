#ifndef CHESSDATABASEINTERFACE_H
#define CHESSDATABASEINTERFACE_H

#include "ChessLinkedListMoves.h"
#include <cstdint>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <pqxx/pqxx>

// Ideas for improvement:
// -- technically all select statements besides the parameter values are know at compile time, so constexpr could do something
// -- use prepared statements
// -- use streams
// -- db settings (more querys?)
// -- using threads (might do nothing because writing to db is the bottleneck?)
// -- optimize way to check if values have to be inserted or updated

using table_pair = std::pair<int, bool>;
using table_move = std::pair<int, MoveCompressed>;
using table_move_ptr = std::pair<int, MoveCompressed *>;

struct DataBitsHash {
    std::size_t operator()(const DataBits &data) const { return data.to_ulong(); }
};
struct TabelUpdateData {
    std::vector<MoveCompressed *> insertMovesDatas;
    std::vector<int> insertFromMoveIds;
    std::vector<table_move_ptr> updateMovesDatas;
};

class ChessDatabaseInterface {
  public:
    using move_tuple = std::tuple<std::string_view, uint64_t, uint64_t, uint64_t>;
    using nexts_ids_map = std::unordered_map<DataBits, int, DataBitsHash>;

    explicit ChessDatabaseInterface(const std::string &dbName);
    ~ChessDatabaseInterface() = default;

    // insertions
    int insertMove(const table_pair &table, const MoveCompressed &move);
    void insertMoves(const table_pair &table, const std::vector<MoveCompressed *> &moves);
    void connectMove(const table_pair &table, int sourceId, int targetId);
    void connectMoves(const table_pair &table, const std::vector<int> &sourceIds, const std::vector<int> &targetIds);

    void updateMove(const table_pair &table, int moveId, int newWins, int newLoses, int newDraws);
    void updateMoves(const table_pair &table, const std::vector<table_move_ptr> &move);

    // getters
    MoveCompressed getMove(const table_pair &table, int moveId);
    MoveCompressed getMove(const table_pair &table, int fromMoveId, const DataBits &moveData);
    int getMoveId(const table_pair &table, int fromMoveId, const DataBits &moveData);
    std::optional<int> getMoveIdOpt(const table_pair &table, int fromMoveId, const DataBits &moveData);
    std::vector<table_move> getNextMoves(const table_pair &table, int moveId);
    nexts_ids_map getNextMovesDataMap(const table_pair &table, int moveId);
    int getMaxIdTable(const table_pair &table);

    void pushMovesToDB(const ChessLinkedListMoves &llMoves);

    void createPreppareds(int depth);

  private:
    pqxx::connection connection;
    std::map<table_pair, std::string> prepUpdates;
    std::map<table_pair, std::string> prepSelectIds;
    const std::string selectMoveFrom = "SELECT moveData, wins, loses, draws FROM ";

    // executes
    pqxx::result executeSQL(pqxx::zview sql, const pqxx::params &pars);
    // queries
    auto queryMove(pqxx::zview sql, const pqxx::params &pars) -> MoveCompressed;
    auto queryMoveId(pqxx::zview sql, const pqxx::params &pars) -> int;
    auto queryMovesToVec(pqxx::zview sql, const pqxx::params &pars) -> std::vector<table_move>;
    auto queryMovesToMap(pqxx::prepped prepSql, const pqxx::params &pars) -> nexts_ids_map;
    // auto queryMovesToMap(pqxx::zview prepSql, const pqxx::params &pars) -> nexts_ids_map;
    void analyze();
    void analyzeTable(const std::string &tableName);

    void fillTableData(const table_move_ptr &currentMoveData, const nexts_ids_map &dbNextMovesMap, TabelUpdateData &updateData);
    void updateTable(table_pair &nextMoveTable, table_pair &connectTable, std::queue<table_move_ptr> &moveQueue,
                     TabelUpdateData &updateData);

  public:
    // some inline funcs
    static std::string joinMoveLinker(const table_pair &tableLT, const table_pair &tableMT) {
        return getChessLinkerTable(tableLT) + " lt JOIN " + getChessMoveTable(tableMT) + " mt ON lt.next_id = mt.id ";
    }
    static char getColor(bool white) { return white ? 'w' : 'b'; }
    static std::string getChessMoveTable(const table_pair &table) {
        return std::format("chess_move_d{}_{}", table.first, getColor(table.second));
    }
    static std::string getChessLinkerTable(const table_pair &table) {
        return std::format("chess_moves_linker_d{}_{}", table.first, getColor(table.second));
    }
    static pqxx::bytes_view getBytesFromDataBits(const DataBits &bits) {
        const auto dataLong = bits.to_ulong();
        const std::byte bytes[2] = {static_cast<std::byte>(dataLong), static_cast<std::byte>(dataLong >> 8)};
        return pqxx::binary_cast(bytes, sizeof(bytes));
    }
    static DataBits getBitsFromDB(std::string_view bytes) {
        unsigned int intMoveData;
        (std::stringstream() << std::hex << bytes) >> intMoveData;
        return {intMoveData};
    }
    static std::string getDBHexStringFromMoveData(const DataBits &bits) {
        return "\\x" + (std::stringstream() << std::hex << std::setw(4) << std::setfill('0') << bits.to_ulong()).str();
    };
};

inline table_pair operator++(table_pair &pair) {
    if (pair.second) pair.first++;
    pair.second = !pair.second;
    return pair;
}

#endif // CHESSDATABASEINTERFACE_H
