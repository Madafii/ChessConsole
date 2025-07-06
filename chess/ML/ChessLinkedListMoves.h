#ifndef CHESSMOVE_H
#define CHESSMOVE_H

#include <array>
#include <bitset>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

enum class RESULT : int8_t { LOSE = -1, DRAW = 0, WIN = 1 };
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
    explicit MoveCompressed(const DataBits inData, uint64_t inWins, uint64_t inLoses, uint64_t inDraws)
        : data(inData), wins(inWins), loses(inLoses), draws(inDraws) {};
    const DataBits data;
    uint64_t wins = 0;
    uint64_t loses = 0;
    uint64_t draws = 0;
    std::vector<std::unique_ptr<MoveCompressed>> nexts;
};

class ChessLinkedListMoves {
  public:
    // constructors
    explicit ChessLinkedListMoves();
    ~ChessLinkedListMoves();

    // adders
    // void addMove(const std::string &nextBoard, const std::string &nextPGN, const std::string &nextMove, const RESULT &result,
    //              bool nextWhite);
    void addMoveCompressed(const std::string &nextMove, const RESULT &result, bool nextWhite);

    void addResult(const RESULT &result);

    // manually set the head to another move
    inline void setMoveHead(MoveCompressed *move) { head = move; };

    // get Move
    [[nodiscard]] inline MoveCompressed *getMoveHead() const { return head; };
    [[nodiscard]] inline MoveCompressed *getMoveRoot() const { return root.get(); };
    [[nodiscard]] MoveCompressed *getAtMove(const std::string &move) const;
    [[nodiscard]] std::vector<const MoveCompressed *> getAllMoves() const;

    // string info of move
    static std::string getFullInfo(const MoveCompressed *move);
    static std::string getInfoMove(const MoveCompressed *move);
    static std::string getBasicInfo(const MoveCompressed *move);
    static std::string getInfoNextMoves(const MoveCompressed *move);

    inline std::string getFullInfoHead() { return getFullInfo(head); };
    inline std::string getInfoMoveHead() { return getInfoMove(head); };
    inline std::string getBasicInfoHead() { return getBasicInfo(head); };
    inline std::string getInfoNextMovesHead() { return getInfoNextMoves(head); };

    // statics
    static inline std::string createKey(const bool &white, const std::string &board) { return (white ? "W|" : "B|") + board; };
    static DataBits createData(const std::string &nextMove, bool nextWhite);
    static std::string getMoveFromData(const DataBits &data);
    static bool getWhiteFromData(const DataBits &data);
    // static inline std::array<char, 2> getCharFromData(const DataBits &data) {
    //     return std::array<char, 2>({static_cast<char>(data.to_ulong() & 0xFF), static_cast<char>((data.to_ulong() >> 8) & 0xFF)});
    // };
    // static inline std::string getStringFromData(const DataBits &data) { return {getCharFromData(data).data()}; };

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
