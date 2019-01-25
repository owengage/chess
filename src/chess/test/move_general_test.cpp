#include <gtest/gtest.h>
#include <chess/game.h>

#include "player_fixture.h"

namespace chess
{
    namespace
    {
        auto constexpr wknight = Square{Knight{Colour::white}};
        auto constexpr bknight = Square{Knight{Colour::black}};
        auto constexpr empty = Square{Empty{}};
    }

    struct MoveGeneralFixture : public PlayerFixture {};
    
    TEST_F(MoveGeneralFixture, can_capture_opposite_piece)
    {
        auto b = Board::blank();
        b["C2"] = wknight;
        b["D4"] = bknight;
        auto g = Game{p1, p2, b};

        EXPECT_TRUE(g.move("C2", "D4"));
        EXPECT_EQ(wknight, g.current()["D4"]);
        EXPECT_EQ(empty, g.current()["C2"]);
    }

    TEST_F(MoveGeneralFixture, cant_capture_own_piece)
    {
        auto b = Board::blank();
        b["C2"] = wknight;
        b["D4"] = wknight;
        auto g = Game{p1, p2, b};

        EXPECT_FALSE(g.move("C2", "D4"));
        EXPECT_EQ(wknight, g.current()["D4"]);
        EXPECT_EQ(wknight, g.current()["C2"]);
    }
}