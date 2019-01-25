#include <iostream>

#include <chess/game.h>
#include <chess/random_player.h>

#include <chess/text/print.h>

using chess::Board;

namespace text = chess::text;

int main() {
    auto p1 = chess::RandomPlayer{};
    auto p2 = chess::RandomPlayer{};
    auto game = chess::Game{p1, p2};
    game.move("C2", "C4");

    text::print(std::cout, game.current());
    std::cout << "Size of square is " << sizeof(chess::Square) << std::endl;
    std::cout << "Size of board is " << sizeof(chess::Board) << std::endl;
    return 0;
}