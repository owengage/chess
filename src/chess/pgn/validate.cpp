#include <chess/pgn/validate.h>
#include <chess/pgn/tokens.h>
#include <chess/pgn/resolve_move.h>

#include <chess/Square.h>
#include <chess/Game.h>
#include <chess/Driver.h>

#include <vector>

using chess::Game;
using chess::Square;
using chess::Move;
using chess::Loc;

using chess::pgn::SanMove;
using chess::pgn::ValidationResult;

namespace
{
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
}

ValidationResult::operator bool() const
{
    return !invalid_move.has_value();
}

ValidationResult chess::pgn::validate(std::vector<SanMove> const &moves)
{
    auto driver = DirectedDriver{};
    auto game = Game{driver};

    for (auto const &san : moves)
    {
        if (san.promotion)
        {
            driver.promotion_choice = Square{*san.promotion, game.current_turn()};
        }

        auto real_move = chess::pgn::resolve_move(san, game.board());

        if (!real_move || MoveType::invalid == game.move(real_move->src, real_move->dest))
        {
            return {san};
        }
    }

    return {};
}