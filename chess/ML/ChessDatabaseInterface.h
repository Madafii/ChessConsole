#ifndef CHESSDATABASEINTERFACE_H
#define CHESSDATABASEINTERFACE_H

#include "ChessLinkedListMoves.h"
#include <pqxx/pqxx>
#include <string>

class ChessDatabaseInterface {
  public:
    using table_pair = std::pair<int, bool>;
    explicit ChessDatabaseInterface(const std::string &dbName);
    ~ChessDatabaseInterface() = default;

    int createMove(const table_pair &table, const MoveCompressed &move);
    void updateMove(const table_pair &table, int moveId, int newWins, int newLoses, int newDraws);
    MoveCompressed getMove(const table_pair &table, int moveId);

    void connectMove(const table_pair &table, int sourceId, int targetId);
    void connectMoves(const table_pair &table, int sourceId, const std::vector<int> &targetIds);
    std::vector<int> getNextMoveIds(int moveId);

    void pushMovesToDB(const MoveCompressed *head, ChessDatabaseInterface::table_pair table);

  private:
    pqxx::connection connection;

    bool executeSQL(const std::string &sql);

    static inline char getColor(bool white) { return white ? 'W' : 'B'; }
    static inline std::string getChessMoveTable(const table_pair &table) {
        return std::format("chess_move_D{}_{}", table.first, getColor(table.second));
    }
    static inline std::string getChessLinkerTable(const table_pair &table) {
        return std::format("chess_moves_linker_D{}_{}", table.first, getColor(table.second));
    }
};

#endif // CHESSDATABASEINTERFACE_H
