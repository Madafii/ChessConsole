#include "ChessLinkedListMoves.h"

#include <format>
#include <iostream>
#include <ranges>

const std::string separatorLine = "------------------------------------------------------------------------------------------\n";

ChessLinkedListMoves::ChessLinkedListMoves() {
    // create the root and set the head to that. TODO: set the original boardStr for the root
    root = std::make_unique<Move>("", "", "", true, nullptr);
    head = root.get();
}

ChessLinkedListMoves::~ChessLinkedListMoves() = default;

/// Add a move and make it the new head
/// @param nextBoard a text representation of the board after the move
/// @param nextPGN the played move in pgn format
/// @param nextMove the played move in from:to format
/// @param result end result of the currently played match
/// @param nextWhite the color that played the move
void ChessLinkedListMoves::addMove(const std::string &nextBoard, const std::string &nextPGN, const std::string &nextMove,
                                   const RESULT &result, const bool nextWhite) {
    const std::string &key = createKey(nextWhite, nextBoard);
    if (head->nexts.contains(key)) {
        head = head->nexts.at(key).get();
        addResult(result);
        return;
    }
    auto newMove = std::make_shared<Move>(nextBoard, nextPGN, nextMove, nextWhite, head);
    head->nexts[key] = std::move(newMove);
    head = head->nexts.at(key).get();
    addResult(result);
}

void ChessLinkedListMoves::addMoveCompressed(const std::string &nextMove, const RESULT &result, bool nextWhite) {}

void ChessLinkedListMoves::addResult(const RESULT &result) {
    // TODO: watched video where atomic_ref was used could work here? When starting to use threads.
    switch (result) {
        case RESULT::WIN:
            head->wins++;
            break;
        case RESULT::LOSE:
            head->loses++;
            break;
        case RESULT::DRAW:
            head->draws++;
            break;
        default:
            break;
    }
}

void ChessLinkedListMoves::addResultCompressed(const RESULT &result) {}

// TODO: I think i don't want this to be able to be nullptr so fix that later
void ChessLinkedListMoves::setMoveHead(Move *move) { head = move; }

Move *ChessLinkedListMoves::getMoveHead() const { return head; }

Move *ChessLinkedListMoves::getMoveRoot() const { return root.get(); }

std::string ChessLinkedListMoves::getKey() const { return createKey(head->white, head->board); }

Move *ChessLinkedListMoves::getAtKey(const std::string &key) const {
    if (!head->nexts.contains(key)) {
        // std::cout << "ChessLinkedListMoves::getAtKey: key not found" << std::endl;
        return nullptr;
    }
    return head->nexts.at(key).get();
}

Move *ChessLinkedListMoves::getAtMove(const std::string &move) const {
    const auto found = std::ranges::find_if(head->nexts, [&move](const auto &pair) { return pair.second.get()->moveName == move; });
    if (found == head->nexts.end()) {
        // std::cout << "ChessLinkedListMoves::getAtMove: move not found" << std::endl;
        return nullptr;
    }
    return found->second.get();
}

Move *ChessLinkedListMoves::getAtPGN(const std::string &pgn) const {
    const auto found = std::ranges::find_if(head->nexts, [&pgn](const auto &pair) { return pair.second.get()->pgnName == pgn; });
    if (found == head->nexts.end()) {
        // std::cout << "ChessLinkedListMoves::getAtPGN: pgn not found" << std::endl;
        return nullptr;
    }
    return found->second.get();
}

std::vector<const Move *> ChessLinkedListMoves::getAllMoves() const {
    std::vector<const Move *> result;
    for (const auto &move : head->nexts | std::ranges::views::values) {
        result.push_back(move.get());
    }
    if (result.empty()) {
        std::cout << "no more nexts moves" << std::endl;
    }
    return result;
}

std::string ChessLinkedListMoves::getInfoMove(const Move *move) {
    std::string output;
    const Move *outputMove = move == nullptr ? head : move;
    output.append("ChessLinkedListMoves: " + outputMove->moveName + "\n");
    output.append(separatorLine);
    output.append("pgn: " + outputMove->pgnName + "\n");
    output.append("board: " + outputMove->board + " | " + (outputMove->white ? "white" : "black") + "\n");
    output.append(separatorLine);
    output.append("wins: " + std::to_string(outputMove->wins) + "\n");
    output.append("loses: " + std::to_string(outputMove->loses) + "\n");
    output.append("draws: " + std::to_string(outputMove->draws) + "\n");
    output.append(separatorLine);
    return output;
}

std::string ChessLinkedListMoves::getBasicInfo(const Move *move) {
    const Move *outputMove = move == nullptr ? head : move;
    return std::format("move: {}, stats(w,l,d): {}, {}, {}\n", outputMove->moveName, outputMove->wins, outputMove->loses,
                       outputMove->draws);
}

std::string ChessLinkedListMoves::getInfoNextMoves(const Move *move) {
    std::string output;
    const Move *outputMove = move == nullptr ? head : move;
    for (const auto &nextMove : outputMove->nexts | std::ranges::views::values) {
        output.append(getBasicInfo(nextMove.get()));
    }
    output.append(separatorLine);
    return output;
}

std::string ChessLinkedListMoves::getFullInfo(const Move *move) {
    std::string output;
    const Move *outputMove = move == nullptr ? head : move;
    output.append(getInfoMove(outputMove));
    output.append(getInfoNextMoves(outputMove));
    return output;
}

std::string ChessLinkedListMoves::createKey(const bool &white, const std::string &board) { return (white ? "W|" : "B|") + board; }

std::bitset<16> ChessLinkedListMoves::createData(const std::string &nextMove, bool nextWhite) {
    std::bitset<16> outSet;
    const std::bitset<3> firstLetter = xToBit.at(nextMove[0]);
    const std::bitset<3> firstNumber = yToBit.at(nextMove[1]);
    const std::bitset<3> secondLetter = xToBit.at(nextMove[2]);
    const std::bitset<3> secondNumber = yToBit.at(nextMove[3]);
    outSet.set(15, firstLetter[2]);
    outSet.set(14, firstLetter[1]);
    outSet.set(13, firstLetter[0]);
    outSet.set(12, firstNumber[2]);
    outSet.set(11, firstNumber[1]);
    outSet.set(10, firstNumber[0]);
    outSet.set(9, secondLetter[2]);
    outSet.set(8, secondLetter[1]);
    outSet.set(7, secondLetter[0]);
    outSet.set(6, secondNumber[2]);
    outSet.set(5, secondNumber[1]);
    outSet.set(4, secondNumber[0]);
    // if pawn won
    if (nextMove.size() > 6) {
        const std::bitset<2> pawnLetter = pawnToBit.at(nextMove[5]);
        outSet.set(3, pawnLetter[1]);
        outSet.set(2, pawnLetter[0]);
    }
    outSet.set(1, nextWhite);

    return outSet;
}

const std::map<char, std::bitset<3>> ChessLinkedListMoves::xToBit{
    {'a', std::bitset<3>("000")}, {'b', std::bitset<3>("001")}, {'c', std::bitset<3>("010")}, {'d', std::bitset<3>("011")},
    {'e', std::bitset<3>("100")}, {'f', std::bitset<3>("101")}, {'g', std::bitset<3>("110")}, {'h', std::bitset<3>("111")}};

const std::map<char, std::bitset<3>> ChessLinkedListMoves::yToBit{
    {'1', std::bitset<3>("000")}, {'2', std::bitset<3>("001")}, {'3', std::bitset<3>("010")}, {'4', std::bitset<3>("011")},
    {'5', std::bitset<3>("100")}, {'6', std::bitset<3>("101")}, {'7', std::bitset<3>("110")}, {'8', std::bitset<3>("111")}};

const std::map<char, std::bitset<2>> ChessLinkedListMoves::pawnToBit {
    {'Q', std::bitset<2>(""), {'R', std::bitset<2>("01")}, {'B', std::bitset<2>("10")}, {'N', std::bitset<2>("11")}};
