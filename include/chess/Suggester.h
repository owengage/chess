#pragma once

#include <chess/Board.h>
#include <chess/Move.h>

#include <functional>

namespace chess
{
    using Score = int;
    using EvalFunc = std::function<Score(Board const&)>;
    struct Move;

    struct Suggester
    {
        Suggester(Board, EvalFunc);

        Move suggest() const;

    private:
        Board m_current;
        EvalFunc m_eval;
    };

    Score evaluate_with_summation(Board const&);
}