#include <algorithm>
#include <cstdint>

#include <chess/game.h>
#include <chess/available_moves.h>
#include <chess/driver.h>

using chess::Loc;
using chess::Game;
using chess::AssumedMoveToken;
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
}

AssumedMoveToken::AssumedMoveToken(Game & game) : game{game}
{}

AssumedMoveToken::~AssumedMoveToken()
{
    game.m_history.pop_back();
}

Game::Game(Driver & driver) : Game{driver, Board::standard()}
{}

Game::Game(Driver & driver, Board b):
    m_start{std::move(b)},
    m_history{},
    m_driver{driver}
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
    auto moves = get_moves_for(*this, src);
    auto move_it = std::find_if(begin(moves), end(moves), [dest](Move m) { return m.dest == dest; });

    if (move_it != std::end(moves))
    {
        auto move = *move_it;
        handle_promotion(move);
        m_history.push_back(move);
        handle_checkmate(move);
        return true;
    }

    return false;
}

AssumedMoveToken Game::assume_move(Move const& move)
{
    m_history.push_back(move);
    return AssumedMoveToken{*this};
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

bool Game::in_check()
{
    return true; // TODO
}

void Game::handle_checkmate(Move const& move)
{
    // Move has been added to game history. Are there any available moves left?
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