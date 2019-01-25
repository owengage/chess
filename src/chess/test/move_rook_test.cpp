#include <gtest/gtest.h>
#include <chess/game.h>

#include "player_fixture.h"

namespace chess
{
    namespace
    {
        auto constexpr wrook = Square{Rook{Colour::white}};
    }

    struct MoveRookFixture : public PlayerFixture
    {
        Game with_wrook_at(Loc loc)
        {
            auto b = Board::blank();
            b[loc] = wrook;
            return Game{p1, p2, b};
        }

        bool try_move(Loc src, Loc dest)
        {
            auto g = with_wrook_at(src);
            return g.move(src, dest);
        }
    };


    TEST_F(MoveRookFixture, can_move_up)
    {
        EXPECT_TRUE(try_move("A1", "A2"));
        EXPECT_TRUE(try_move("A1", "A3"));
        EXPECT_TRUE(try_move("A1", "A4"));
        EXPECT_TRUE(try_move("A1", "A5"));
        EXPECT_TRUE(try_move("A1", "A6"));
        EXPECT_TRUE(try_move("A1", "A7"));
        EXPECT_TRUE(try_move("A1", "A8"));
    }

    TEST_F(MoveRookFixture, cant_move_up_through_own_piece)
    {
        auto game = Game{p1, p2, Board::with_pieces({
            {"C1", wrook},
            {"C5", wrook},
        })};

        EXPECT_FALSE(game.move("C1", "C6"));
    }

    TEST_F(MoveRookFixture, can_capture_up)
    {
        auto game = Game{p1, p2, Board::with_pieces({
            {"C1", wrook},
            {"C5", Square{Rook{Colour::black}}}
        })};

        EXPECT_TRUE(game.move("C1", "C5"));
    }

    TEST_F(MoveRookFixture, can_move_down)
    {
        EXPECT_TRUE(try_move("A8", "A1"));
        EXPECT_TRUE(try_move("A8", "A2"));
        EXPECT_TRUE(try_move("A8", "A3"));
        EXPECT_TRUE(try_move("A8", "A4"));
        EXPECT_TRUE(try_move("A8", "A5"));
        EXPECT_TRUE(try_move("A8", "A6"));
        EXPECT_TRUE(try_move("A8", "A7"));
    }

    TEST_F(MoveRookFixture, can_move_left)
    {
        EXPECT_TRUE(try_move("H4", "G4"));
        EXPECT_TRUE(try_move("H4", "F4"));
        EXPECT_TRUE(try_move("H4", "E4"));
        EXPECT_TRUE(try_move("H4", "D4"));
        EXPECT_TRUE(try_move("H4", "C4"));
        EXPECT_TRUE(try_move("H4", "B4"));
        EXPECT_TRUE(try_move("H4", "A4"));
    }

    TEST_F(MoveRookFixture, can_move_right)
    {
        EXPECT_TRUE(try_move("A4", "H4"));
        EXPECT_TRUE(try_move("A4", "G4"));
        EXPECT_TRUE(try_move("A4", "F4"));
        EXPECT_TRUE(try_move("A4", "E4"));
        EXPECT_TRUE(try_move("A4", "D4"));
        EXPECT_TRUE(try_move("A4", "C4"));
        EXPECT_TRUE(try_move("A4", "B4"));
    }
}