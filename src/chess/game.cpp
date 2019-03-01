#include <algorithm>
#include <cstdint>

#include <chess/game.h>
#include <chess/available_moves.h>
#include <chess/driver.h>

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

namespace
{
    std::vector<Move> get_moves_for(Game const &game, Loc src)
    {
        auto moves = chess::available_moves(game);
        auto it = std::remove_if(begin(moves), end(moves), [&src](auto move) { return move.src != src; });
        moves.erase(it, end(moves));

        return moves;
    }

    bool is_pawn_double_jump(Move const& move)
    {
        auto const& sq = move.result[move.dest];
        auto const dy = move.src.y() - move.dest.y();

        return std::holds_alternative<Pawn>(sq) && std::abs(dy) == 2;
    }
}

Game::Game(Driver & driver) : Game{driver, Board::standard()}
{}

Game::Game(Driver & driver, Board b):
    m_board{std::move(b)},
    m_turn{Colour::white},
    m_driver{driver}
{}

Board Game::current() const
{
    return m_board;
}

Colour Game::current_turn() const
{
    return m_turn;
}

std::optional<Loc> const& Game::last_turn_pawn_double_jump_dest() const
{
    return m_last_turn_pawn_double_jump_dest;
}

bool Game::move(Loc src, Loc dest)
{
    auto moves = get_moves_for(*this, src);
    auto move_it = std::find_if(begin(moves), end(moves), [dest](Move m) { return m.dest == dest; });

    if (move_it != std::end(moves))
    {
        force_move(*move_it);
        handle_checkmate(*move_it);
        return true;
    }

    return false;
}

void Game::force_move(Move move)
{
    handle_promotion(move);
    m_turn = flip_colour(m_turn);
    m_board = move.result;

    if (is_pawn_double_jump(move))
    {
        m_last_turn_pawn_double_jump_dest = move.dest;
    }
    else
    {
        m_last_turn_pawn_double_jump_dest = std::nullopt;
    }
}

void Game::handle_promotion(chess::Move & move)
{
    auto const y = move.dest.y();
    auto const p = move.result[move.dest];

    if ((y == Loc::side_size - 1 || y == 0) && std::holds_alternative<Pawn>(p))
    {
        auto colour = get_colour(p);
        auto promotion = m_driver.promote(*this, move);

        // FIXME: What if they returned empty.
        if (colour == get_colour(promotion) && !std::holds_alternative<Pawn>(promotion))
        {
            move.result[move.dest] = promotion;
        }
        else
        {
            throw InvalidDriverAction{"Promoted to invalid piece"};
        }
    }
}

void Game::handle_checkmate(Move const& move)
{
    auto moves = available_moves(*this);
    if (moves.empty())
    {
        if (move.caused_check)
        {
            m_driver.checkmate(*this, move);
        }
        else
        {
            m_driver.stalemate(*this, move);
        }
    }
}