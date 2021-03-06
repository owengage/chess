#include <gtest/gtest.h>
#include <chess/Game.h>

#include "game_fixture.h"

namespace chess
{
    namespace
    {
        auto constexpr wbishop = Bishop(Colour::white);
    }

    struct MoveBishopFixture : public GameFixture
    {
        Game with_wbishop_at(Loc loc)
        {
            auto b = Board::blank();
            b[loc] = wbishop;
            return Game{driver, b};
        }

        bool try_move(Loc src, Loc dest)
        {
            auto g = with_wbishop_at(src);
            return MoveType::stalemate == g.move(src, dest);
        }
    };


    TEST_F(MoveBishopFixture, can_move_upright)
    {
        EXPECT_TRUE(try_move("A1", "B2"));
        EXPECT_TRUE(try_move("A1", "C3"));
        EXPECT_TRUE(try_move("A1", "D4"));
        EXPECT_TRUE(try_move("A1", "E5"));
        EXPECT_TRUE(try_move("A1", "F6"));
        EXPECT_TRUE(try_move("A1", "G7"));
        EXPECT_TRUE(try_move("A1", "H8"));
    }

    TEST_F(MoveBishopFixture, can_move_downleft)
    {
        EXPECT_TRUE(try_move("H8", "A1"));
        EXPECT_TRUE(try_move("H8", "B2"));
        EXPECT_TRUE(try_move("H8", "C3"));
        EXPECT_TRUE(try_move("H8", "D4"));
        EXPECT_TRUE(try_move("H8", "E5"));
        EXPECT_TRUE(try_move("H8", "F6"));
        EXPECT_TRUE(try_move("H8", "G7"));
    }

    TEST_F(MoveBishopFixture, can_move_upleft)
    {
        EXPECT_TRUE(try_move("H1", "G2"));
        EXPECT_TRUE(try_move("H1", "F3"));
        EXPECT_TRUE(try_move("H1", "E4"));
        EXPECT_TRUE(try_move("H1", "D5"));
        EXPECT_TRUE(try_move("H1", "C6"));
        EXPECT_TRUE(try_move("H1", "B7"));
        EXPECT_TRUE(try_move("H1", "A8"));
    }

    TEST_F(MoveBishopFixture, can_move_downright)
    {
        EXPECT_TRUE(try_move("A8", "H1"));
        EXPECT_TRUE(try_move("A8", "G2"));
        EXPECT_TRUE(try_move("A8", "F3"));
        EXPECT_TRUE(try_move("A8", "E4"));
        EXPECT_TRUE(try_move("A8", "D5"));
        EXPECT_TRUE(try_move("A8", "C6"));
        EXPECT_TRUE(try_move("A8", "B7"));
    }

    TEST_F(MoveBishopFixture, cant_move_through_own_piece)
    {
        auto game = Game{driver, Board::with_pieces({
            {"B2", wbishop},
            {"D4", wbishop},
        })};

        EXPECT_EQ(MoveType::invalid, game.move("B2", "E5"));
    }

    TEST_F(MoveBishopFixture, can_capture)
    {
        auto game = Game{driver, Board::with_pieces({
            {"B2", wbishop},
            {"D4", Bishop(Colour::black)}
        })};

        EXPECT_EQ(MoveType::stalemate, game.move("B2", "D4"));
    }

}