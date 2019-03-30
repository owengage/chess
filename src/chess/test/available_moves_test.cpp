#include <chess/game.h>
#include <chess/available_moves.h>

#include "game_fixture.h"

#include <gtest/gtest.h>

MATCHER_P2(contains_loc_with_sq, loc, sq, "")
{
    std::vector<chess::Move> const& moves = arg;
    auto it = std::find_if(begin(moves), end(moves), [this](chess::Move move)
    {
        return move.result[loc] == sq;
    });

    return it != end(moves);
}

namespace chess
{
    struct AvailableMovesFixture : public GameFixture
    {
    };

    TEST_F(AvailableMovesFixture, standard_start_should_have_20_moves)
    {
        auto game = Game{driver};
        auto moves = available_moves(game.board());
        EXPECT_EQ(20, moves.size()); // 8 pawns with two moves each. 2 knights two moves each == 20
    }

    TEST_F(AvailableMovesFixture, pawn_getting_promoted_should_have_move_for_each_royal_piece)
    {
        auto game = Game{driver, Board::with_pieces({
                {"A7", Pawn(Colour::white)}
        })};

        auto moves = available_moves(game.board());
        EXPECT_EQ(4, moves.size());
        EXPECT_THAT(moves, contains_loc_with_sq("A8", Rook(Colour::white)));
        EXPECT_THAT(moves, contains_loc_with_sq("A8", Bishop(Colour::white)));
        EXPECT_THAT(moves, contains_loc_with_sq("A8", Knight(Colour::white)));
        EXPECT_THAT(moves, contains_loc_with_sq("A8", Queen(Colour::white)));
    }
}