#ifndef CHESSDATABASEINTERFACE_H
#define CHESSDATABASEINTERFACE_H

#include "ChessLinkedListMoves.h"
#include <cstdint>
#include <pqxx/pqxx>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <utility>

class ChessDatabaseInterface {
  public:
    using table_pair = std::pair<int, bool>;
    using move_tuple = std::tuple<std::string_view, uint64_t, uint64_t, uint64_t>;

    explicit ChessDatabaseInterface(const std::string &dbName);
    ~ChessDatabaseInterface() = default;

    std::optional<int> createMove(const table_pair &table, const MoveCompressed &move);
    void updateMove(const table_pair &table, int moveId, int newWins, int newLoses, int newDraws);
    std::optional<MoveCompressed> getMove(const table_pair &table, int moveId);
    std::optional<MoveCompressed> getMove(const table_pair &table, int fromMoveId, const DataBits &moveData);
    std::optional<int> getMoveId(const table_pair &table, int fromMoveId, const DataBits &moveData);
    std::vector<MoveCompressed> getNextMoves(const table_pair &table, int moveId);

    void connectMove(const table_pair &table, int sourceId, int targetId);
    void connectMoves(const table_pair &table, int sourceId, const std::vector<int> &targetIds);

    void pushMovesToDB(const ChessLinkedListMoves &llMoves, ChessDatabaseInterface::table_pair table);

  private:
    pqxx::connection connection;

    std::optional<pqxx::result> executeSQL(const std::string &sql, const pqxx::params &pars);
    std::optional<MoveCompressed> queryMove(const std::string &sql, const pqxx::params &pars);
    std::optional<int> queryMoveId(const std::string &sql, const pqxx::params &pars);
    std::vector<MoveCompressed> queryMoves(const std::string &sql, const pqxx::params &pars);

    const std::string selectCompressedMoveFrom = "SELECT moveData, wins, loses, draws FROM ";

    static inline std::string joinMoveLinker(const table_pair &tableLT, const table_pair &tableMT) {
        return getChessLinkerTable(tableLT) + " lt JOIN " + getChessMoveTable(tableMT) + " mt ON lt.next_id = mt.id ";
    }

  public:
    // some inline funcs
    static inline char getColor(bool white) { return white ? 'W' : 'B'; }
    static inline std::string getChessMoveTable(const table_pair &table) {
        return std::format("chess_move_D{}_{}", table.first, getColor(table.second));
    }
    static inline std::string getChessLinkerTable(const table_pair &table) {
        return std::format("chess_moves_linker_D{}_{}", table.first, getColor(table.second));
    }
    static inline DataBits getBitsFromDB(std::string_view bytes) {
        unsigned int intMoveData;
        (std::stringstream() << std::hex << bytes) >> intMoveData;
        return {intMoveData};
    }
    static inline std::string getDBHexStringFromMoveData(const DataBits &bits) {
        return "\\x" + (std::stringstream() << std::hex << std::setw(4) << std::setfill('0') << bits.to_ulong()).str();
    };
    static inline void incrementTable(table_pair &pair) {
        if (pair.second) pair.first++;
        pair.second = !pair.second;
    }
};

#endif // CHESSDATABASEINTERFACE_H
