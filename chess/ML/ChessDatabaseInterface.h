#ifndef CHESSDATABASEINTERFACE_H
#define CHESSDATABASEINTERFACE_H

#include "ChessLinkedListMoves.h"
#include <pqxx/pqxx>
#include <string>
#include <string_view>
#include <tuple>

class ChessDatabaseInterface {
  public:
    using table_pair = std::pair<int, bool>;
    // they are saved in this way in the db to save storage
    /*using small_table_tuple = std::tuple<std::byte[2], uint16_t, uint16_t, uint16_t>;*/
    /*using medium_table_tuple = std::tuple<std::byte[2], uint32_t, uint16_t, uint16_t>;*/
    /*using big_table_tuple = std::tuple<std::byte[2], uint64_t, uint16_t, uint16_t>;*/

    explicit ChessDatabaseInterface(const std::string &dbName);
    ~ChessDatabaseInterface() = default;

    std::optional<int> createMove(const table_pair &table, const MoveCompressed &move);
    void updateMove(const table_pair &table, int moveId, int newWins, int newLoses, int newDraws);
    MoveCompressed getMove(const table_pair &table, int moveId);

    void connectMove(const table_pair &table, int sourceId, int targetId);
    void connectMoves(const table_pair &table, int sourceId, const std::vector<int> &targetIds);
    std::vector<int> getNextMoveIds(int moveId);

    void pushMovesToDB(const ChessLinkedListMoves &llMoves, ChessDatabaseInterface::table_pair table);

  private:
    pqxx::connection connection;

    std::optional<pqxx::result> executeSQL(const std::string &sql, const pqxx::params &pars);

    static inline char getColor(bool white) { return white ? 'W' : 'B'; }
    static inline std::string getChessMoveTable(const table_pair &table) {
        return std::format("chess_move_D{}_{}", table.first, getColor(table.second));
    }
    static inline std::string getChessLinkerTable(const table_pair &table) {
        return std::format("chess_moves_linker_D{}_{}", table.first, getColor(table.second));
    }

    static DataBits getBitsFromDB(std::string_view bytes);
};

#endif // CHESSDATABASEINTERFACE_H
