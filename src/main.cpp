#include <iostream>

#include <chess/game.h>
#include <chess/text/print.h>

using chess::Board;

namespace text = chess::text;

int main() {
    auto game = chess::Game{};
    game.move("C2", "C4");

    text::print(std::cout, game.current());
    std::cout << "Size of square is " << sizeof(chess::Square) << std::endl;
    std::cout << "Size of board is " << sizeof(chess::Board) << std::endl;
    return 0;
}