#include <chess/game.h>
#include <chess/driver.h>
#include <chess/pgn/move_parser.h>
#include <chess/pgn/resolve_move.h>

#include <iostream>
#include <optional>

using chess::Game;
using chess::Move;
using chess::Square;
using chess::pgn::MoveParser;
using chess::pgn::IncompleteGameError;
using chess::pgn::SanMove;

namespace
{
    struct DirectedDriver : chess::Driver
    {
        Square promote(Game const& game, Move const& move) override
        {
            return chess::Square();
        }

        void checkmate(Game const& game, Move const& move) override
        {

        }

        void stalemate(Game const& game, Move const& move) override
        {

        }

        std::optional<Square> promotion_choice = std::nullopt;
    };

    void validate(std::vector<SanMove> const& moves)
    {
        auto driver = DirectedDriver{};
        auto game = Game{driver};

        for (auto const& san : moves)
        {
            auto real_move = chess::pgn::resolve_move(san, game);

            if (real_move)
            {
                // TODO: Check returns true.
                game.move(real_move.src, real_move.dest);
            }
            else
            {
                // TODO: Cheated!
            }
        }
    }
}

int main(int argc, char const ** argv)
{
    auto parser = MoveParser{std::cin};

    while (auto moves = parser.next_game())
    {
        validate(moves);
    }
}