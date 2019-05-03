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
    m_eval{std::move(eval_func)},
    m_tree{Evaluation{0, Move{"A1", "A1", m_current}}}
{
    auto depth = 4;
    build_tree(m_tree, depth);
}

void Suggester::build_tree(Tree<Evaluation> & root, int depth)
{
    auto const& board = root.value().move.result;

    if (depth == 0)
    {
        root.value().score = m_eval(root.value().move);
        return;
    }

    auto moves = available_moves(board);

    if (moves.empty())
    {
        root.value().score = m_eval(root.value().move);
        return;
    }

    for (auto const& move : moves)
    {
        auto & child = root.add_child(Evaluation{0, move});
        build_tree(child, depth - 1);
    }

    // Layer below us is already evaluated due to recursion
    auto const& children = root.children();
    auto score = Score{};
    auto maximise = [](Score current, Score child) { return std::max(current, child); };
    auto minimise = [](Score current, Score child) { return std::min(current, child); };
    auto compare = root.value().move.result.turn == Colour::white ? maximise : minimise;

    for (auto const& child : children)
    {
        score = compare(child.value().score, score);
    }

    root.value().score = score;
}

Move Suggester::suggest() const
{
    Evaluation const * min = nullptr;
    Evaluation const * max = nullptr;

    for (auto const& child : m_tree.children())
    {
        auto score = child.value().score;
        if (!min || score < min->score)
        {
            min = &child.value();
        }

        if (!max || score > max->score)
        {
            max = &child.value();
        }
    }

    if (!min || !max)
    {
        throw std::runtime_error{"Found no moves."};
    }

    return m_current.turn == Colour::white ? max->move : min->move;
}

Score chess::evaluate_with_summation(Move const& move)
{
    Score score = 0;

    for (auto const& loc : Loc::all_squares())
    {
        score += score_square(move.result[loc]);
        score += (move.type == MoveType::check) ? 10 : 0;
        score += (move.type == MoveType::checkmate) ? 10000 : 0;
    }

    return score;
}