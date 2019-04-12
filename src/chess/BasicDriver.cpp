#include <chess/BasicDriver.h>
#include <chess/Move.h>
#include <chess/Game.h>

using chess::BasicDriver;
using chess::Square;

Square BasicDriver::promote(Game const&, Move const& move)
{
    auto const& sq = move.result[move.dest];
    return Queen(sq.colour());
}

void BasicDriver::checkmate(Game const&, Move const&)
{

}

void BasicDriver::stalemate(Game const&, Move const&)
{

}