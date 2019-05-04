#include <chess/Game.h>
#include <chess/Driver.h>
#include <chess/pgn/MoveParser.h>
#include <chess/pgn/resolve_move.h>
#include <chess/pgn/validate.h>

#include <chess/text/print.h>

#include <iostream>
#include <optional>

using chess::Game;
using chess::Move;
using chess::Square;
using chess::Loc;
using chess::pgn::MoveParser;
using chess::pgn::IncompleteGameError;
using chess::pgn::SanMove;

struct DirectedDriver : chess::Driver
{
    Square promote(Game const& game, Move const& move) override
    {
        return *promotion_choice;
    }

    void checkmate(Game const& game, Move const& move) override
    {

    }

    void stalemate(Game const& game, Move const& move) override
    {

    }

    std::optional<Square> promotion_choice = std::nullopt;
};

std::string to_string(Loc loc)
{
    return std::string(1, loc.x() + 'A') + std::to_string(loc.y() + 1);
}

std::string to_string(SanMove const& san)
{
    return san.original_text;
}

void go_through_game(std::vector<SanMove> const& moves)
{
    auto driver = DirectedDriver{};
    auto game = Game{driver};
    for (auto const& san : moves)
    {
        if (san.promotion)
        {
            driver.promotion_choice = Square{*san.promotion, game.current_turn()};
        }

        auto real_move = chess::pgn::resolve_move(san, game.board());

        if (real_move && game.move(real_move->src, real_move->dest) != chess::MoveType::invalid)
        {
            std::cout << "Move: " << to_string(san) << "\n";
            chess::text::print(std::cout, game.board());
            std::cout << '\n';
            //std::getchar();
        }
        else
        {
            std::cout << "Invalid move: " << to_string(san) << "\n";
            exit(1);
        }
    }
}

int main(int argc, char const ** argv)
{
    auto parser = MoveParser{std::cin};
    auto game_count = 0;
    while (auto moves = parser.next_game())
    {
        game_count++;
        std::cout << "Validating game " << game_count << '\n';
        auto result = chess::pgn::validate(*moves);
        if (!result)
        {
            std::cerr << "Invalid move: " << to_string(*result.invalid_move) << '\n';
            for (auto const& move : *moves)
            {
                std::cerr << to_string(move) << " ";
            }
            exit(1);
        }
    }
}