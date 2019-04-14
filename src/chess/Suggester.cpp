#include <chess/Suggester.h>
#include <chess/available_moves.h>

using chess::Suggester;
using chess::SquareType;
using chess::Square;
using chess::Move;
using chess::Score;
using chess::Colour;

namespace
{
    Score score_type(SquareType type)
    {
        switch (type)
        {
            case SquareType::empty:
                return 0;
            case SquareType::pawn:
                return 1;
            case SquareType::rook:
            case SquareType::bishop:
            case SquareType::knight:
                return 3;
            case SquareType::queen:
                return 5;
            case SquareType::king:
                return 1000;
        }
    }

    Score score_square(Square sq)
    {
        auto base = score_type(sq.type());
        return sq.colour() == Colour::white ? base : -base;
    }
}

Suggester::Suggester(Board board, EvalFunc eval_func) :
    m_current{std::move(board)},
    m_eval{std::move(eval_func)}
{}

Move Suggester::suggest() const
{
    auto moves = available_moves(m_current);
    auto min_max_it = std::minmax_element(begin(moves), end(moves), [this](Move const& lhs, Move const& rhs)
    {
        return m_eval(lhs.result) < m_eval(rhs.result);
    });

    return m_current.turn == Colour::white ? *min_max_it.second : *min_max_it.first;
}

Score chess::evaluate_with_summation(Board const& board)
{
    Score score = 0;

    for (auto const& loc : Loc::all_squares())
    {
        score += score_square(board[loc]);
    }

    return score;
}