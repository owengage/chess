#include <algorithm>
#include <cstdint>

#include <chess/Game.h>
#include <chess/available_moves.h>
#include <chess/Driver.h>

using chess::Loc;
using chess::Game;
using chess::MoveType;
using chess::Player;
using chess::Board;
using chess::Square;
using chess::Colour;
using chess::Move;
using chess::SquareType;

namespace
{
    std::vector<Move> get_moves_for(Game const &game, Loc src)
    {
        auto moves = chess::available_moves(game.board());
        auto it = std::remove_if(begin(moves), end(moves), [&src](auto move) { return move.src != src; });
        moves.erase(it, end(moves));

        return moves;
    }
}

Game::Game(Driver & driver) : Game{driver, Board::standard()}
{}

Game::Game(Driver & driver, Board b):
    m_board{std::move(b)},
    m_driver{driver}
{}

Board Game::board() const
{
    return m_board;
}

Colour Game::current_turn() const
{
    return m_board.turn;
}

MoveType Game::move(Loc src, Loc dest)
{
    auto moves = get_moves_for(*this, src);
    auto move_it = std::find_if(begin(moves), end(moves), [dest](Move m) { return m.dest == dest; });

    if (move_it != std::end(moves))
    {
        handle_promotion(*move_it);
        m_board = move_it->result;
        return handle_mate(*move_it);
    }

    return MoveType::invalid;
}

void Game::handle_promotion(chess::Move & move)
{
    auto const p = move.result[move.dest];

    if (move.is_promotion)
    {
        auto colour = p.colour();
        auto promotion = m_driver.promote(*this, move);
        auto type = promotion.type();

        if (type != SquareType::empty && type != SquareType::pawn && colour == promotion.colour())
        {
            move.result[move.dest] = promotion;
        }
        else
        {
            throw InvalidDriverAction{"Promoted to invalid piece"};
        }
    }
}

MoveType Game::handle_mate(Move const &move)
{
    auto moves = available_moves(m_board);
    if (moves.empty())
    {
        if (move.caused_check)
        {
            m_driver.checkmate(*this, move);
            return MoveType::checkmate;
        }
        else
        {
            m_driver.stalemate(*this, move);
            return MoveType::stalemate;
        }
    }

    return MoveType::normal;
}