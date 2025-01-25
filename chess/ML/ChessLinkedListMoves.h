#ifndef CHESSMOVE_H
#define CHESSMOVE_H

#include <bitset>
#include <cstring>
#include <memory>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <utility>
#include <vector>

enum class RESULT { LOSE = -1, DRAW = 0, WIN = 1 };
using DataBits = std::bitset<16>;

// white|board is the key
// TODO: maybe should change this to shared pointer(done),but still do -> Reason -> the key
// should only exist once in the whole tree but multiple branches could reach it (maybe solve it by merging
// afterwards)

/*struct Move {*/
/*    Move(std::string board, std::string pgn, std::string move, const bool &white, const Move *previousMove)*/
/*        : board(std::move(board)), pgnName(std::move(pgn)), moveName(std::move(move)), white(white) , prev{previousMove} {}*/
/*    const std::string board;*/
/*    const std::string pgnName;*/
/*    const std::string moveName;*/
/*    const bool white;*/
/*    u_int32_t wins = 0;*/
/*    u_int32_t loses = 0;*/
/*    u_int32_t draws = 0;*/
/*    const Move *prev = nullptr; */
/*    std::unordered_map<const std::string, std::shared_ptr<Move>> nexts;*/
/*};*/

struct MoveCompressed {
    explicit MoveCompressed(const DataBits inData) : data{inData} {};
    const DataBits data;
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
    // void addMove(const std::string &nextBoard, const std::string &nextPGN, const std::string &nextMove, const RESULT &result,
    //              bool nextWhite);
    void addMoveCompressed(const std::string &nextMove, const RESULT &result, bool nextWhite);

    void addResult(const RESULT &result);

    // manually set the head to another move
    void setMoveHead(MoveCompressed *move);

    // get Move
    [[nodiscard]] MoveCompressed *getMoveHead() const;
    [[nodiscard]] MoveCompressed *getMoveRoot() const;
    [[nodiscard]] MoveCompressed *getAtMove(const std::string &move) const;
    [[nodiscard]] std::vector<const MoveCompressed *> getAllMoves() const;

    // info by move, not set uses head
    std::string getFullInfo(const MoveCompressed *move = nullptr);
    std::string getInfoMove(const MoveCompressed *move = nullptr);
    std::string getBasicInfo(const MoveCompressed *move = nullptr);
    std::string getInfoNextMoves(const MoveCompressed *move = nullptr);

    // statics
    static std::string createKey(const bool &white, const std::string &board);
    static DataBits createData(const std::string &nextMove, bool nextWhite);
    static std::string getMoveFromData(const DataBits &data);
    static bool getWhiteFromData(const DataBits &data);

    // bit maps
    static const std::unordered_map<char, std::bitset<3>> xToBit;
    static const std::unordered_map<char, std::bitset<3>> yToBit;
    static const std::unordered_map<char, std::bitset<2>> pawnToBit;
    static const std::unordered_map<std::bitset<3>, char> BitToX;
    static const std::unordered_map<std::bitset<3>, char> BitToY;
    static const std::unordered_map<std::bitset<2>, char> BitToPawn;

  private:
    MoveCompressed *head = nullptr;
    std::unique_ptr<MoveCompressed> root;
};

#endif
