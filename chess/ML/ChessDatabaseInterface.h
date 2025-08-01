#ifndef CHESSDATABASEINTERFACE_H
#define CHESSDATABASEINTERFACE_H

#include "ChessLinkedListMoves.h"
#include <cstdint>
#include <pqxx/pqxx>
#include <queue>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <unordered_map>
#include <utility>

// Ideas for improvement:
// -- technically all select statements besides the parameter values are know at compile time, so constexpr could do something
// -- use prepared statements
// -- use streams
// -- db settings (more querys?)
// -- using threads (might do nothing because writing to db is the bottleneck?)
// -- optimize way to check if values have to be inserted or updated

using table_pair = std::pair<int, bool>;

struct DataBitsHash {
    std::size_t operator()(const DataBits &data) const { return static_cast<std::size_t>(data.to_ulong()); }
};
struct TabelUpdateData {
    std::vector<MoveCompressed *> insertMovesDatas;
    std::vector<int> insertFromMoveIds;
    std::vector<std::pair<int, MoveCompressed *>> updateMovesDatas;
};

class ChessDatabaseInterface {
  public:
    using move_tuple = std::tuple<std::string_view, uint64_t, uint64_t, uint64_t>;
    using nexts_ids_map = std::unordered_map<DataBits, int, DataBitsHash>;

    explicit ChessDatabaseInterface(const std::string &dbName);
    ~ChessDatabaseInterface() = default;

    // insertions
    int insertMove(const table_pair &table, const MoveCompressed &move);
    void insertMoves(const table_pair &table, const std::vector<MoveCompressed *> &move);
    void connectMove(const table_pair &table, int sourceId, int targetId);
    void connectMoves(const table_pair &table, int sourceId, const std::vector<int> &targetIds);
    void connectMoves(const table_pair &table, const std::vector<int> &sourceIds, const std::vector<int> &targetIds);

    void updateMove(const table_pair &table, int moveId, int newWins, int newLoses, int newDraws);
    void updateMoves(const table_pair &table, const std::vector<std::pair<int, MoveCompressed *>> &move);

    // getters
    MoveCompressed getMove(const table_pair &table, int moveId);
    MoveCompressed getMove(const table_pair &table, int fromMoveId, const DataBits &moveData);
    int getMoveId(const table_pair &table, int fromMoveId, const DataBits &moveData);
    std::vector<std::pair<int, MoveCompressed>> getNextMoves(const table_pair &table, int moveId);
    nexts_ids_map getNextMovesDataMap(const table_pair &table, int moveId);
    int getMaxIdTable(const table_pair &table);

    void pushMovesToDB(const ChessLinkedListMoves &llMoves);

  private:
    pqxx::connection connection;

    // querys
    pqxx::result executeSQL(const std::string &sql, const pqxx::params &pars);
    MoveCompressed queryMove(const std::string &sql, const pqxx::params &pars);
    int queryMoveId(const std::string &sql, const pqxx::params &pars);
    std::vector<std::pair<int, MoveCompressed>> queryMovesToVec(const std::string &sql, const pqxx::params &pars);
    nexts_ids_map queryMovesToMap(const std::string &sql, const pqxx::params &pars);

    void fillTableData(const std::pair<int, MoveCompressed *> &currentMoveData, const nexts_ids_map &dbNextMovesMap, TabelUpdateData &updateData);
    void updateTable(table_pair &nextMoveTable, table_pair &connectTable, std::queue<std::pair<int, MoveCompressed *>> &moveQueue,
                     TabelUpdateData &updateData);

    const std::string selectMoveFrom = "SELECT moveData, wins, loses, draws FROM ";

    static inline std::string joinMoveLinker(const table_pair &tableLT, const table_pair &tableMT) {
        return getChessLinkerTable(tableLT) + " lt JOIN " + getChessMoveTable(tableMT) + " mt ON lt.next_id = mt.id ";
    }

  public:
    // some inline funcs
    static inline char getColor(bool white) { return white ? 'w' : 'b'; }
    static inline std::string getChessMoveTable(const table_pair &table) {
        return std::format("chess_move_d{}_{}", table.first, getColor(table.second));
    }
    static inline std::string getChessLinkerTable(const table_pair &table) {
        return std::format("chess_moves_linker_d{}_{}", table.first, getColor(table.second));
    }
    static inline pqxx::bytes_view getBytesFromDataBits(const DataBits &bits) {
        const auto dataLong = bits.to_ulong();
        const std::byte bytes[2] = {static_cast<std::byte>(dataLong), static_cast<std::byte>(dataLong >> 8)};
        return pqxx::binary_cast(bytes, sizeof(bytes));
    }
    static inline DataBits getBitsFromDB(std::string_view bytes) {
        unsigned int intMoveData;
        (std::stringstream() << std::hex << bytes) >> intMoveData;
        return {intMoveData};
    }
    static inline std::string getDBHexStringFromMoveData(const DataBits &bits) {
        return "\\x" + (std::stringstream() << std::hex << std::setw(4) << std::setfill('0') << bits.to_ulong()).str();
    };
};

inline table_pair operator++(table_pair &pair) {
    if (pair.second) pair.first++;
    pair.second = !pair.second;
    return pair;
}

#endif // CHESSDATABASEINTERFACE_H
