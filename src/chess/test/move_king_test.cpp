#include <gtest/gtest.h>
#include <chess/game.h>

#include "player_fixture.h"

namespace chess
{
    namespace
    {
        auto constexpr wking = Square{King{Colour::white}};
    }


    struct MoveKingFixture : public PlayerFixture
    {
        Game with_wking_at(Loc loc)
        {
            auto b = Board::blank();
            b[loc] = wking;
            return Game{p1, p2, b};
        }

        bool try_move(Loc src, Loc dest)
        {
            auto g = with_wking_at(src);
            return g.move(src, dest);
        }
    };

    TEST_F(MoveKingFixture, can_move_one_square)
    {
        EXPECT_TRUE(try_move("C4", "C5"));
        EXPECT_TRUE(try_move("C4", "D5"));
        EXPECT_TRUE(try_move("C4", "D4"));
        EXPECT_TRUE(try_move("C4", "D3"));
        EXPECT_TRUE(try_move("C4", "C3"));
        EXPECT_TRUE(try_move("C4", "B3"));
        EXPECT_TRUE(try_move("C4", "B4"));
        EXPECT_TRUE(try_move("C4", "B5"));
    }

    TEST_F(MoveKingFixture, can_capture)
    {
        auto game = Game{p1, p2, Board::with_pieces({
            {"B2", wking},
            {"B3", Square{Knight{Colour::black}}}
        })};

        EXPECT_TRUE(game.move("B2", "B3"));
    }

}