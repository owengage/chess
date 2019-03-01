#include <iostream>

#include <chess/game.h>
#include <chess/basic_driver.h>

#include <chess/text/print.h>

using chess::Board;

namespace text = chess::text;

int main() {
    auto driver = chess::BasicDriver{};
    auto game = chess::Game{driver};
    game.move("C2", "C4");

    text::print(std::cout, game.current());
    std::cout << "Size of square is " << sizeof(chess::Square) << std::endl;
    std::cout << "Size of board is " << sizeof(chess::Board) << std::endl;
    std::cout << "Size of board is " << sizeof(chess::Game) << std::endl;
    return 0;
}