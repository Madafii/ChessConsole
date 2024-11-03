//
// Created by fpittermann on 10/5/24.
//

#include "ChessTile.h"

ChessTile::ChessTile(std::unique_ptr<ChessPiece> piece, const int x, const int y) : piece(std::move(piece)), _x(x), _y(y) {

}

std::string ChessTile::getMove() const {
    return std::string(mapIntToX.at(_x) + std::to_string(_y + 1));
}

const std::unordered_map<char, int> ChessTile::mapXtoInt = {
        {'a', 0}, {'b', 1}, {'c', 2}, {'d', 3},
        {'e', 4}, {'f', 5}, {'g', 6}, {'h', 7}
};

const std::unordered_map<int, char> ChessTile::mapIntToX = []() {
    std::unordered_map<int, char> ret;
    for (const auto&[fst, snd] : mapXtoInt) {
        ret[snd] = fst;
    }
    return ret;
}();


