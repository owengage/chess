#include <chess/basic_driver.h>
#include <chess/move.h>
#include <chess/game.h>

using chess::BasicDriver;
using chess::Square;

Square BasicDriver::promote(Game const&, Move const& move)
{
    auto const& sq = move.result[move.dest];
    return Queen(get_colour(sq));
}

void BasicDriver::checkmate(Game const&, Move const&)
{

}

void BasicDriver::stalemate(Game const&, Move const&)
{

}