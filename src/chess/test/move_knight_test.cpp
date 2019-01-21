#include <gtest/gtest.h>
#include <chess/game.h>

namespace chess
{
    namespace
    {
        auto constexpr wknight = Square{Knight{Colour::white}};

        Game with_wknight_at(Loc loc)
        {
            auto b = Board::blank();
            b[loc] = wknight;
            return Game{b};
        }

        bool try_move(Loc src, Loc dest)
        {
            auto g = with_wknight_at(src);
            return g.move(src, dest);
        }
    }

    TEST(move_knight_test, can_move_white_knight)
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

    TEST(move_knight_test, can_jump)
    {
        auto b = Board::blank();
        for (auto loc : Loc::all_squares())
        {
            b[loc] = Square{Pawn{Colour::black}};
        }

        b["A1"] = wknight;
        auto g = Game{b};

        EXPECT_TRUE(g.move("A1", "B3"));
    }
}