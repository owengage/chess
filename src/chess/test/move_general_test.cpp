#include <gtest/gtest.h>
#include <chess/game.h>

namespace chess
{
    namespace
    {
        auto constexpr wknight = Square{Knight{Colour::white}};
        auto constexpr bknight = Square{Knight{Colour::black}};
        auto constexpr empty = Square{Empty{}};
    }

    TEST(move_general_test, can_capture_opposite_piece)
    {
        auto b = Board::blank();
        b["C2"] = wknight;
        b["D4"] = bknight;
        auto g = Game{b};

        EXPECT_TRUE(g.move("C2", "D4"));
        EXPECT_EQ(wknight, g.current()["D4"]);
        EXPECT_EQ(empty, g.current()["C2"]);
    }

    TEST(move_general_test, cant_capture_own_piece)
    {
        auto b = Board::blank();
        b["C2"] = wknight;
        b["D4"] = wknight;
        auto g = Game{b};

        EXPECT_FALSE(g.move("C2", "D4"));
        EXPECT_EQ(wknight, g.current()["D4"]);
        EXPECT_EQ(wknight, g.current()["C2"]);
    }
}