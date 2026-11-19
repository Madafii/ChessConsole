#include "ChessWebInterface.h"

#include <exception>
#include <iostream>

int main() {
    try {
        ChessWebInterface{}.run();
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
