#include <chess/square.h>
#include <chess/random_player.h>
#include <chess/game.h>

using chess::Square;
using chess::RandomPlayer;

Square RandomPlayer::promote(const chess::Game & game, chess::Loc src)
{
    auto c = get_colour(game.current()[src]);
    return Square{Queen{c}};
}