#include <gtest/gtest.h>
#include <chess/game.h>

#include "game_fixture.h"

namespace chess
{
    namespace
    {
        auto constexpr wqueen = Queen(Colour::white);
    }

    struct MoveQueenFixture : public GameFixture
    {
        Game with_wqueen_at(Loc loc)
        {
            auto b = Board::blank();
            b[loc] = wqueen;
            return Game{driver, b};
        }

        bool try_move(Loc src, Loc dest)
        {
            auto g = with_wqueen_at(src);
            return g.move(src, dest);
        }
    };
    
    TEST_F(MoveQueenFixture, can_move_upright)
    {
        EXPECT_TRUE(try_move("A1", "B2"));
        EXPECT_TRUE(try_move("A1", "C3"));
        EXPECT_TRUE(try_move("A1", "D4"));
        EXPECT_TRUE(try_move("A1", "E5"));
        EXPECT_TRUE(try_move("A1", "F6"));
        EXPECT_TRUE(try_move("A1", "G7"));
        EXPECT_TRUE(try_move("A1", "H8"));
    }

    TEST_F(MoveQueenFixture, can_move_downleft)
    {
        EXPECT_TRUE(try_move("H8", "A1"));
        EXPECT_TRUE(try_move("H8", "B2"));
        EXPECT_TRUE(try_move("H8", "C3"));
        EXPECT_TRUE(try_move("H8", "D4"));
        EXPECT_TRUE(try_move("H8", "E5"));
        EXPECT_TRUE(try_move("H8", "F6"));
        EXPECT_TRUE(try_move("H8", "G7"));
    }

    TEST_F(MoveQueenFixture, can_move_upleft)
    {
        EXPECT_TRUE(try_move("H1", "G2"));
        EXPECT_TRUE(try_move("H1", "F3"));
        EXPECT_TRUE(try_move("H1", "E4"));
        EXPECT_TRUE(try_move("H1", "D5"));
        EXPECT_TRUE(try_move("H1", "C6"));
        EXPECT_TRUE(try_move("H1", "B7"));
        EXPECT_TRUE(try_move("H1", "A8"));
    }

    TEST_F(MoveQueenFixture, can_move_downright)
    {
        EXPECT_TRUE(try_move("A8", "H1"));
        EXPECT_TRUE(try_move("A8", "G2"));
        EXPECT_TRUE(try_move("A8", "F3"));
        EXPECT_TRUE(try_move("A8", "E4"));
        EXPECT_TRUE(try_move("A8", "D5"));
        EXPECT_TRUE(try_move("A8", "C6"));
        EXPECT_TRUE(try_move("A8", "B7"));
    }
    
    TEST_F(MoveQueenFixture, can_move_up)
    {
        EXPECT_TRUE(try_move("A1", "A2"));
        EXPECT_TRUE(try_move("A1", "A3"));
        EXPECT_TRUE(try_move("A1", "A4"));
        EXPECT_TRUE(try_move("A1", "A5"));
        EXPECT_TRUE(try_move("A1", "A6"));
        EXPECT_TRUE(try_move("A1", "A7"));
        EXPECT_TRUE(try_move("A1", "A8"));
    }
    
    TEST_F(MoveQueenFixture, can_move_down)
    {
        EXPECT_TRUE(try_move("A8", "A1"));
        EXPECT_TRUE(try_move("A8", "A2"));
        EXPECT_TRUE(try_move("A8", "A3"));
        EXPECT_TRUE(try_move("A8", "A4"));
        EXPECT_TRUE(try_move("A8", "A5"));
        EXPECT_TRUE(try_move("A8", "A6"));
        EXPECT_TRUE(try_move("A8", "A7"));
    }

    TEST_F(MoveQueenFixture, can_move_left)
    {
        EXPECT_TRUE(try_move("H4", "G4"));
        EXPECT_TRUE(try_move("H4", "F4"));
        EXPECT_TRUE(try_move("H4", "E4"));
        EXPECT_TRUE(try_move("H4", "D4"));
        EXPECT_TRUE(try_move("H4", "C4"));
        EXPECT_TRUE(try_move("H4", "B4"));
        EXPECT_TRUE(try_move("H4", "A4"));
    }

    TEST_F(MoveQueenFixture, can_move_right)
    {
        EXPECT_TRUE(try_move("A4", "H4"));
        EXPECT_TRUE(try_move("A4", "G4"));
        EXPECT_TRUE(try_move("A4", "F4"));
        EXPECT_TRUE(try_move("A4", "E4"));
        EXPECT_TRUE(try_move("A4", "D4"));
        EXPECT_TRUE(try_move("A4", "C4"));
        EXPECT_TRUE(try_move("A4", "B4"));
    }


    TEST_F(MoveQueenFixture, cant_move_through_own_piece)
    {
        auto game = Game{driver, Board::with_pieces({
            {"B2", wqueen},
            {"D4", wqueen},
        })};

        EXPECT_FALSE(game.move("B2", "E5"));
    }

    TEST_F(MoveQueenFixture, can_capture)
    {
        auto game = Game{driver, Board::with_pieces({
            {"B2", wqueen},
            {"D4", Queen(Colour::black)}
        })};

        EXPECT_TRUE(game.move("B2", "D4"));
    }

}