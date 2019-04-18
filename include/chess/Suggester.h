#pragma once

#include <chess/Board.h>
#include <chess/Move.h>
#include <chess/Tree.h>

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
        struct Evaluation
        {
            Score score;
            Move move;
        };

        Board m_current;
        EvalFunc m_eval;
        Tree<Evaluation> m_tree;

        void build_tree(Tree<Evaluation> & root, int depth);
    };

    Score evaluate_with_summation(Board const&);
}