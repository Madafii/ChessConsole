#ifndef CHESSDATABASEINTERFACE_H
#define CHESSDATABASEINTERFACE_H

#include "ChessLinkedListMoves.h"
#include <string>
#include <sqlite3.h>

class ChessDatabaseInterface {
public:
    explicit ChessDatabaseInterface(const std::string& dbName);
    ~ChessDatabaseInterface();

    bool createNode(const MoveCompressed &move);
    bool connectNodes(int sourceId, int targetId);

    void printNodes() const;

    [[nodiscard]] std::vector<std::unique_ptr<MoveCompressed>> getMoves() const;

private:
    sqlite3* db;
    bool executeSQL(const std::string& sql);
};



#endif //CHESSDATABASEINTERFACE_H
