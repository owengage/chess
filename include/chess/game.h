#pragma once

#include <vector>

#include <chess/board.h>
#include <chess/move.h>

namespace chess
{
    struct Player;

    struct Game
    {
        Game(Player &, Player &);
        Game(Player &, Player &, Board);

        Board current() const;
        std::vector<Move> const& history() const;

        bool move(Loc src, Loc dest);
    private:
        Board m_start;
        std::vector<Move> m_history;
        Player & m_player1;
        Player & m_player2;

        Player & current_player();
        void handle_promotion(Move &);
    };
}