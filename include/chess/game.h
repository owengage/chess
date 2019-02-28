#pragma once

#include <vector>

#include <chess/board.h>
#include <chess/move.h>

namespace chess
{
    struct Player;
    struct Game;
    struct Driver;

    struct Game
    {
        Game(Driver &);
        Game(Driver &, Board);

        Board current() const;

        bool move(Loc src, Loc dest);
        void force_move(Move);

        Colour current_turn() const;
        std::optional<Loc> const& last_move_destination() const;

    private:
        Board m_board;
        Colour m_turn;
        std::optional<Loc> m_last_move_destination;
        Driver & m_driver;

        void handle_promotion(Move &);
        void handle_checkmate(Move const&);

        friend struct AssumedMoveToken;
    };
}