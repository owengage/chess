#include <algorithm>
#include <cstdint>

#include <chess/game.h>
#include <chess/player.h>
#include <chess/move_generator.h>

using chess::Loc;
using chess::Game;
using chess::Player;
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

Game::Game(Player & p1, Player & p2) : Game{p1, p2, Board::standard()}
{}

Game::Game(Player & p1, Player & p2, Board b):
    m_start{std::move(b)},
    m_history{},
    m_player1{p1},
    m_player2{p2}
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
        auto move = *move_it;
        handle_promotion(move);
        m_history.push_back(move);
        return true;
    }

    return false;
}

Player & Game::current_player()
{
    return (m_history.size() % 2 == 0) ? m_player1 : m_player2;
}

void Game::handle_promotion(chess::Move & move)
{
    auto const y = move.dest.y();
    auto const p = move.result[move.dest];

    if ((y == Loc::side_size - 1 || y == 0) && std::holds_alternative<Pawn>(p))
    {
        auto & player = current_player();
        auto colour = get_colour(p);
        auto promotion = player.promote(*this, move.dest);

        if (colour == get_colour(promotion) && !std::holds_alternative<Pawn>(promotion))
        {
            move.result[move.dest] = promotion;
        }
        else
        {
            throw InvalidPlayerAction{"Promoted to invalid piece"};
        }
    }
}