#include <chess/Game.h>
#include <chess/Suggester.h>
#include <chess/Driver.h>

#include <chess/text/print.h>

#include <iostream>
#include <string>

using chess::Square;
using chess::Game;
using chess::Move;
using chess::MoveType;

namespace
{
    struct PlayerDriver : chess::Driver
    {
        Square promote(Game const& game, Move const& move) override
        {
            // TODO: Let player pick promotion
            auto sq = move.result[move.dest];
            return chess::Queen(sq.colour());
        }

        void checkmate(Game const& game, Move const& move) override
        {
        }

        void stalemate(Game const& game, Move const& move) override
        {
        }

    };

    std::ostream & operator<<(std::ostream & os, MoveType const& movetype)
    {
        switch (movetype)
        {
            case MoveType::invalid:
                os << "invalid";
            case MoveType::normal:
                os << "normal";
            case MoveType::checkmate:
                os << "checkmate";
            case MoveType::stalemate:
                os << "stalemate";
        }
        return os;
    }
}

int main()
{
    auto last_move = chess::MoveType::normal;
    auto driver = PlayerDriver{};
    auto game = Game{driver};
    auto move_src = std::string{};
    auto move_dest = std::string{};

    while (last_move == MoveType::normal)
    {
//        do
//        {
//            std::cout << "Enter move source: ";
//            std::cin >> move_src;
//
//            std::cout << "Enter move destination: ";
//            std::cin >> move_dest;
//
//            last_move = game.move(move_src.c_str(), move_dest.c_str());
//            if (last_move == MoveType::invalid)
//            {
//                std::cout << "Incorrect move, try again.\n";
//            }
//        }
//        while (last_move == MoveType::invalid);

        chess::text::print(std::cout, game.board());

        auto suggester = chess::Suggester{game.board(), chess::evaluate_with_summation};
        auto suggestion = suggester.suggest();
        last_move = game.move(suggestion.src, suggestion.dest);
    }

    std::cout << "Game ended with move type of " << last_move << '\n';
}