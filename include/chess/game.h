#pragma once

#include <vector>

#include <chess/board.h>
#include <chess/move.h>

namespace chess
{
    struct Game
    {
        Game();
        Game(Board);

        Board current() const;
        std::vector<Move> const& history() const;

        bool move(Loc src, Loc dest);
    private:
        Board m_start;
        std::vector<Move> m_history;
    };
}