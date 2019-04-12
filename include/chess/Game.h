#pragma once

#include <vector>

#include <chess/Board.h>
#include <chess/Move.h>

namespace chess
{
    struct Player;
    struct Game;
    struct Driver;

    struct Game
    {
        Game(Driver &);
        Game(Driver &, Board);

        Board board() const;

        bool move(Loc src, Loc dest);

        Colour current_turn() const;
        std::optional<Loc> const& last_turn_pawn_double_jump_dest() const;

    private:
        Board m_board;
        Driver & m_driver;

        void handle_promotion(Move &);
        void handle_checkmate(Move const&);
    };
}