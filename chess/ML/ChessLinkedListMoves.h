#ifndef CHESSMOVE_H
#define CHESSMOVE_H

#include <bitset>
#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <sys/types.h>
#include <utility>
#include <vector>

enum class RESULT { LOSE = -1, DRAW = 0, WIN = 1 };

// white|board is the key
// TODO: maybe should change this to shared pointer(done),but still do -> Reason -> the key
// should only exist once in the whole tree but multiple branches could reach it (maybe solve it by merging
// afterwards)
struct Move {
    Move(std::string board, std::string pgn, std::string move, const bool &white /*, const Move *previousMove*/)
        : board(std::move(board)), pgnName(std::move(pgn)), moveName(std::move(move)), white(white) /*, prev{previousMove}*/ {}
    const std::string board;
    const std::string pgnName;
    const std::string moveName;
    const bool white;
    u_int32_t wins = 0;
    u_int32_t loses = 0;
    u_int32_t draws = 0;
    /*const Move *prev = nullptr;*/
    std::map<const std::string, std::shared_ptr<Move>> nexts;
};

struct MoveCompressed {
    explicit MoveCompressed(const std::bitset<16> inData) : data{inData} {};
    const std::bitset<16> data;
    u_int64_t wins = 0;
    u_int64_t loses = 0;
    u_int64_t draws = 0;
    std::vector<std::unique_ptr<MoveCompressed>> nexts;
};

class ChessLinkedListMoves {
  public:
    explicit ChessLinkedListMoves();
    ~ChessLinkedListMoves();

    // adders
    void addMove(const std::string &nextBoard, const std::string &nextPGN, const std::string &nextMove, const RESULT &result,
                 bool nextWhite);
    void addMoveCompressed(const std::string &nextMove, const RESULT &result, bool nextWhite);

    void addResult(const RESULT &result);

    // manually set the head to another move
    void setMoveHead(Move *move);

    // get Move
    [[nodiscard]] Move *getMoveHead() const;
    [[nodiscard]] Move *getMoveRoot() const;
    [[nodiscard]] Move *getAtKey(const std::string &key) const;
    [[nodiscard]] Move *getAtMove(const std::string &move) const;
    [[nodiscard]] Move *getAtPGN(const std::string &pgn) const;
    [[nodiscard]] std::vector<const Move *> getAllMoves() const;

    // info by move, not set uses head
    std::string getFullInfo(const Move *move = nullptr);
    std::string getInfoMove(const Move *move = nullptr);
    std::string getBasicInfo(const Move *move = nullptr);
    std::string getInfoNextMoves(const Move *move = nullptr);

    [[nodiscard]] std::string getKey() const;

    // statics
    static std::string createKey(const bool &white, const std::string &board);
    static std::bitset<16> createData(const std::string &nextMove, bool nextWhite);

    // bit maps
    static const std::map<char, std::bitset<3>> xToBit;
    static const std::map<char, std::bitset<3>> yToBit;
    static const std::map<char, std::bitset<2>> pawnToBit;

    // reverse maps
    static constexpr std::map<std::bitset<3>, char> BitToX;
    static constexpr std::map<std::bitset<3>, char> BitToY;
    static constexpr std::map<std::bitset<2>, char> BitToPawn;

  private:
    MoveCompressed *head = nullptr;
    std::unique_ptr<MoveCompressed> root;

};

#endif
