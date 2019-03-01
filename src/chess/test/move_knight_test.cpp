#include <gtest/gtest.h>
#include <chess/game.h>

#include "game_fixture.h"

using testing::_;
using testing::Return;

namespace chess
{
    namespace
    {
        auto constexpr wknight = Square{Knight{Colour::white}};
    }

    struct MoveKnightFixture : public GameFixture
    {
        Game with_wknight_at(Loc loc)
        {
            auto b = Board::blank();
            b[loc] = wknight;
            return Game{driver, b};
        }

        bool try_move(Loc src, Loc dest)
        {
            auto g = with_wknight_at(src);
            return g.move(src, dest);
        }
    };

    TEST_F(MoveKnightFixture, can_move_white_knight)
    {
        EXPECT_TRUE(try_move("A5", "B7"));
        EXPECT_TRUE(try_move("A5", "B3"));
        EXPECT_TRUE(try_move("A5", "C6"));
        EXPECT_TRUE(try_move("A5", "C4"));
        EXPECT_TRUE(try_move("D5", "B6"));
        EXPECT_TRUE(try_move("D5", "B4"));
        EXPECT_TRUE(try_move("D5", "C7"));
        EXPECT_TRUE(try_move("D5", "C3"));
    }

    TEST_F(MoveKnightFixture, can_jump)
    {
        // FIXME: This is only necessary because the move generation force_move function handles promotion. Should
        // probably make this part of the move generation.
        EXPECT_CALL(driver, promote(_, _)).WillRepeatedly(Return(Square{Queen{Colour::black}}));

        auto b = Board::blank();
        for (auto loc : Loc::all_squares())
        {
            b[loc] = Square{Pawn{Colour::black}};
        }

        b["A1"] = wknight;
        auto g = Game{driver, b};

        EXPECT_TRUE(g.move("A1", "B3"));
    }
}