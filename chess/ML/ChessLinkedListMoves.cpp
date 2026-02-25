#include "ChessLinkedListMoves.h"

#include <cstddef>
#include <format>
#include <iostream>
#include <string>

const std::string separatorLine = "------------------------------------------------------------------------------------------\n";

ChessLinkedListMoves::ChessLinkedListMoves() {
    // create the root and set the head to that.
    constexpr std::bitset<16> emptyInData;
    root = std::make_unique<MoveCompressed>(emptyInData);
    head = root.get();
}

ChessLinkedListMoves::~ChessLinkedListMoves() = default;

void ChessLinkedListMoves::addMoveCompressed(const std::string &nextMove, const RESULT &result, bool nextWhite) {
    const std::bitset<16> inData = createData(nextMove, nextWhite);
    const auto findData = std::ranges::find_if(head->nexts, [&inData](auto &move) { return move->data == inData; });
    if (findData != head->nexts.end()) {
        head = findData->get();
        addResult(result);
        return;
    }
    auto newCompressedMove = std::make_unique<MoveCompressed>(inData);
    head->nexts.push_back(std::move(newCompressedMove));
    head = head->nexts.back().get();
    addResult(result);
}

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

MoveCompressed *ChessLinkedListMoves::getAtMove(const std::string &move) const {
    const auto found = std::ranges::find_if(head->nexts, [&move](const auto &nextMove) { return getMoveFromData(nextMove->data) == move; });
    if (found == head->nexts.end()) {
        return nullptr;
    }
    return found->get();
}

std::vector<const MoveCompressed *> ChessLinkedListMoves::getAllMoves() const {
    std::vector<const MoveCompressed *> result;
    result.reserve(head->nexts.size());
    for (const auto &move : head->nexts) {
        result.push_back(move.get());
    }
    if (result.empty()) {
        std::cout << "no more nexts moves" << std::endl;
    }
    return result;
}

std::string ChessLinkedListMoves::getFullInfo(const MoveCompressed *move) {
    std::string output;
    output.append(getInfoMove(move));
    output.append(getInfoNextMoves(move));
    return output;
}

std::string ChessLinkedListMoves::getInfoMove(const MoveCompressed *move) {
    std::string output;
    output.append("move:  " + getMoveFromData(move->data) + "\n");
    output.append(separatorLine);
    output.append("color: " + std::string(getWhiteFromData(move->data) ? "white" : "black") + "\n");
    output.append(separatorLine);
    output.append("wins:  " + std::to_string(move->wins) + "\n");
    output.append("loses: " + std::to_string(move->loses) + "\n");
    output.append("draws: " + std::to_string(move->draws) + "\n");
    output.append(separatorLine);
    return output;
}

std::string ChessLinkedListMoves::getBasicInfo(const MoveCompressed *move) {
    return std::format("move: {}, stats(w,l,d): {}, {}, {}\n", getMoveFromData(move->data), move->wins, move->loses, move->draws);
}

std::string ChessLinkedListMoves::getInfoNextMoves(const MoveCompressed *move) {
    std::string output;
    for (const auto &nextMove : move->nexts) {
        output.append(getBasicInfo(nextMove.get()));
    }
    output.append(separatorLine);
    return output;
}

std::bitset<16> ChessLinkedListMoves::createData(std::string_view nextMove, bool nextWhite) {
    std::bitset<16> outSet;

    auto setBits = [&](const std::bitset<3> &bits, const size_t startPos) {
        for (size_t i = 0; i < 3; ++i) {
            outSet.set(startPos + i, bits[i]);
        }
    };

    setBits(xToBit.at(nextMove[0]), 13); // Set firstLetter
    setBits(yToBit.at(nextMove[1]), 10); // Set firstNumber
    setBits(xToBit.at(nextMove[3]), 7);  // Set secondLetter
    setBits(yToBit.at(nextMove[4]), 4);  // Set secondNumber

    // pawn won
    if (nextMove.size() == 5) {
        outSet.reset(3); // mark if pawn won
    } else {
        const std::bitset<2> pawnLetter = pawnToBit.at(nextMove[6]);
        outSet.set(2, pawnLetter[1]);
        outSet.set(1, pawnLetter[0]);
    }
    outSet.set(0, nextWhite);
    // is wrong way in db so check that

    return outSet;
}

std::string ChessLinkedListMoves::getMoveFromData(const DataBits &data) {
    std::string outMove;

    auto retrieveDataX = [&outMove](const DataBits &dataBits, const size_t startPos) {
        std::bitset<3> bits;
        for (size_t i = 0; i < 3; ++i) {
            bits.set(i, dataBits[startPos + i]);
        }
        outMove += BitToX.at(bits);
    };
    auto retrieveDataY = [&outMove](const DataBits &dataBits, const size_t startPos) {
        std::bitset<3> bits;
        for (size_t i = 0; i < 3; ++i) {
            bits.set(i, dataBits[startPos + i]);
        }
        outMove += BitToY.at(bits);
    };

    retrieveDataX(data, 13); // first letter
    retrieveDataY(data, 10); // first number
    outMove.append(":");     // sepearator
    retrieveDataX(data, 7);  // second letter
    retrieveDataY(data, 4);  // second number

    // pawn won
    if (data.test(3)) {
        std::bitset<2> pawnLetter;
        pawnLetter.set(1, data[2]);
        pawnLetter.set(0, data[1]);
        outMove.append("=" + std::to_string(BitToPawn.at(pawnLetter)));
    }
    return outMove;
}

bool ChessLinkedListMoves::getWhiteFromData(const DataBits &data) { return data.test(15); }

const std::unordered_map<char, std::bitset<3>> ChessLinkedListMoves::xToBit{
    {'a', std::bitset<3>("000")}, {'b', std::bitset<3>("001")}, {'c', std::bitset<3>("010")}, {'d', std::bitset<3>("011")},
    {'e', std::bitset<3>("100")}, {'f', std::bitset<3>("101")}, {'g', std::bitset<3>("110")}, {'h', std::bitset<3>("111")}};

const std::unordered_map<char, std::bitset<3>> ChessLinkedListMoves::yToBit{
    {'1', std::bitset<3>("000")}, {'2', std::bitset<3>("001")}, {'3', std::bitset<3>("010")}, {'4', std::bitset<3>("011")},
    {'5', std::bitset<3>("100")}, {'6', std::bitset<3>("101")}, {'7', std::bitset<3>("110")}, {'8', std::bitset<3>("111")}};

const std::unordered_map<char, std::bitset<2>> ChessLinkedListMoves::pawnToBit{
    {'Q', std::bitset<2>("00")}, {'R', std::bitset<2>("01")}, {'B', std::bitset<2>("10")}, {'N', std::bitset<2>("11")}};

const std::unordered_map<std::bitset<3>, char> ChessLinkedListMoves::BitToX = []() {
    std::unordered_map<std::bitset<3>, char> outMap;
    for (const auto &[letter, bits] : xToBit) {
        outMap[bits] = letter;
    }
    return outMap;
}();

const std::unordered_map<std::bitset<3>, char> ChessLinkedListMoves::BitToY = []() {
    std::unordered_map<std::bitset<3>, char> outMap;
    for (const auto &[letter, bits] : yToBit) {
        outMap[bits] = letter;
    }
    return outMap;
}();

const std::unordered_map<std::bitset<2>, char> ChessLinkedListMoves::BitToPawn = []() {
    std::unordered_map<std::bitset<2>, char> outMap;
    for (const auto &[letter, bits] : pawnToBit) {
        outMap[bits] = letter;
    }
    return outMap;
}();
