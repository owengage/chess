#include <algorithm>
#include <cstdint>

#include <chess/game.h>
#include <chess/move_generator.h>

using chess::Loc;
using chess::Game;
using chess::Board;
using chess::Square;
using chess::Empty;
using chess::PieceCommon;
using chess::Pawn;
using chess::Rook;
using chess::Knight;
using chess::Bishop;
using chess::Queen;
using chess::King;
using chess::Colour;
using chess::Move;
using chess::MoveList;
using chess::MoveGenerator;

namespace
{
    MoveList get_moves(Game const &game, Loc src)
    {
        auto sq = game.current()[src];
        auto pv = MoveGenerator{game, src};
        return std::visit(pv, sq);
    }
}

Game::Game() : Game{Board::standard()}
{}

Game::Game(Board b) : m_start{std::move(b)}
{}

Board Game::current() const
{
    if (!m_history.empty())
    {
        return m_history.back().result;
    }
    return m_start;
}

std::vector<Move> const& Game::history() const
{
    return m_history;
}

bool Game::move(Loc src, Loc dest)
{
    auto moves = get_moves(*this, src);
    auto move_it = std::find_if(begin(moves), end(moves), [dest](Move m) { return m.dest == dest; });

    if (move_it != std::end(moves))
    {
        m_history.push_back(*move_it);
        return true;
    }

    return false;
}
