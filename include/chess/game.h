#pragma once

#include <vector>

#include <chess/board.h>
#include <chess/move.h>

namespace chess
{
    struct Player;
    struct Game;
    struct Driver;

    struct AssumedMoveToken
    {
        AssumedMoveToken(Game &);
        ~AssumedMoveToken();
        AssumedMoveToken(AssumedMoveToken const&) = delete;
        void operator=(AssumedMoveToken const&) = delete;

        Game & game;
    };

    struct Game
    {
        Game(Driver &);
        Game(Driver &, Board);

        Board current() const;
        std::vector<Move> const& history() const;

        bool move(Loc src, Loc dest);
        Colour current_turn() const;

        /**
         * Make a move that will be taken back once the returned token goes out of scope. No validation done.
         */
        AssumedMoveToken assume_move(Move const&);
    private:
        Board m_start;
        std::vector<Move> m_history;
        Driver & m_driver;

        bool in_check();
        void handle_promotion(Move &);
        void handle_checkmate(Move const&);

        friend struct AssumedMoveToken;
    };
}