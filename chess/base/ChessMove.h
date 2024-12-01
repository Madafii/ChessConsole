#ifndef CHESSMOVE_H
#define CHESSMOVE_H

#include <map>
#include <memory>
#include <string>

enum class RESULT { LOSE = -1, DRAW = 0, WIN = 1 };

class ChessMove {
  public:
    explicit ChessMove(const std::string &board, const std::string &pgn, const std::string &move,
                       bool white, const RESULT &result);
    ~ChessMove();

    ChessMove *addNext(const std::string &nextBoard, const std::string &nextPGN, const std::string &nextMove,
                       const RESULT &result, bool nextWhite);
    void addResult(const RESULT &result);
    std::string getKey() const;
    static std::string createKey(const bool &white, const std::string &board);

  private:
    // white|board is the key
    // TODO: maybe should change this to shared pointer(done),but still do -> Reason -> the key
    // should only exist once in the whole tree but multiple branches could reach it
    std::map<const std::string, std::shared_ptr<ChessMove>> nexts;
    const std::string board;
    const std::string pgnName;
    const std::string moveName;
    const bool white;
    u_int32_t wins;
    u_int32_t loses;
    u_int32_t draws;
};

inline std::string ChessMove::createKey(const bool &white, const std::string &board) {
    return (white ? "W|" : "B|") + board;
}

#endif
