#ifndef CHESSMOVE_H
#define CHESSMOVE_H

#include <string>
#include <map>

enum class Result {
    LOSE=-1,
    DRAW=0,
    WIN=1
};

class ChessMove {
public:
    explicit ChessMove(const std::string &board, const std::string &pgn, const std::string &move, bool white);
    ~ChessMove();

    void addNext(const std::string &board, const std::string &pgn, const std::string &move, const Result &result,
                 bool white);
    std::string getKey() const;

private:
    // white|board is the key
    std::map<const std::string, ChessMove*> nexts;
    const std::string board;
    const std::string pgnName;
    const std::string moveName;
    const bool white;
    u_int32_t wins;
    u_int32_t loses;
    u_int32_t draws;
};

#endif
