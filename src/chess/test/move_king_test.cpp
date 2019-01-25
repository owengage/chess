#include <gtest/gtest.h>
#include <chess/game.h>

namespace chess
{
    namespace
    {
        auto constexpr wking = Square{King{Colour::white}};

        bool try_move(Loc src, Loc dest)
        {
            auto g = Game{Board::with_pieces({
                {src, wking}
            })};
            return g.move(src, dest);
        }
    }

    TEST(move_king_test, can_move_one_square)
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

    TEST(move_king_test, can_capture)
    {
        auto game = Game{Board::with_pieces({
            {"B2", wking},
            {"B3", Square{Knight{Colour::black}}}
        })};

        EXPECT_TRUE(game.move("B2", "B3"));
    }

}