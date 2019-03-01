#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <chess/game.h>
#include <chess/text/print.h>

#include "game_fixture.h"

using testing::_;
using testing::Return;

namespace chess
{
    namespace
    {
        auto constexpr wpawn = Pawn(Colour::white);
        auto constexpr bpawn = Pawn(Colour::black);
    }

    struct MovePawnFixture : public GameFixture
    {
        Game with_wpawn_at(Loc loc)
        {
            auto b = Board::blank();
            b[loc] = wpawn;
            return Game{driver, b};
        }
    };
    
    TEST_F(MovePawnFixture, can_move_white_pawn_forward_one)
    {
        auto g = Game{driver};
        EXPECT_TRUE(g.move("A2", "A3"));
        EXPECT_EQ(wpawn, g.current()["A3"]);
    }

    TEST_F(MovePawnFixture, cant_move_white_pawn_left_right_down)
    {
        auto g = Game{driver};
        EXPECT_FALSE(g.move("A2", "B2"));
        EXPECT_FALSE(g.move("B2", "A2"));
        EXPECT_FALSE(g.move("A2", "A1"));
    }

    TEST_F(MovePawnFixture, cant_move_pawn_if_not_players_turn)
    {
        auto g = Game{driver};
        g.move("A2", "A3");
        EXPECT_FALSE(g.move("A3", "A4"));
    }

    TEST_F(MovePawnFixture, cant_move_beyond_end)
    {
        auto g = with_wpawn_at("A8");
        EXPECT_THROW(g.move("A8", "A9"), LocInvalid);
    }

    TEST_F(MovePawnFixture, cant_move_no_space)
    {
        auto g = Game{driver};
        EXPECT_FALSE(g.move("A2", "A2"));
    }

    TEST_F(MovePawnFixture, cant_move_no_space_at_board_end)
    {
        auto g = with_wpawn_at("D8");
        EXPECT_FALSE(g.move("D8", "D8"));
    }

    TEST_F(MovePawnFixture, cant_diagonal_to_empty)
    {
        auto g = with_wpawn_at("B1");
        EXPECT_FALSE(g.move("B1", "C2"));
        EXPECT_FALSE(g.move("B1", "A2"));
    }

    TEST_F(MovePawnFixture, can_right_diagonal_to_opposite_piece)
    {
        auto b = Board::blank();
        b["A1"] = wpawn;
        b["B2"] = bpawn;

        auto g = Game{driver, b};
        EXPECT_TRUE(g.move("A1", "B2"));
    }

    TEST_F(MovePawnFixture, can_left_diagonal_to_opposite_piece)
    {
        auto b = Board::blank();
        b["B1"] = wpawn;
        b["A2"] = bpawn;

        auto g = Game{driver, b};
        EXPECT_TRUE(g.move("B1", "A2"));
    }

    TEST_F(MovePawnFixture, cant_capture_forward)
    {
        auto b = Board::blank();
        b["B1"] = wpawn;
        b["B2"] = bpawn;

        auto g = Game{driver, b};
        EXPECT_FALSE(g.move("B1", "B2"));
    }

    TEST_F(MovePawnFixture, cant_double_jump_after_own_first_move)
    {
        auto g = Game{driver};
        g.move("A2", "A3");
        EXPECT_FALSE(g.move("A5", "A5"));
    }

    TEST_F(MovePawnFixture, can_double_jump_on_own_first_move)
    {
        auto g = Game{driver};
        EXPECT_TRUE(g.move("A2", "A4"));
    }

    TEST_F(MovePawnFixture, en_passant_left)
    {
        auto b = Board::with_pieces({
            {"A2", wpawn},
            {"B4", bpawn}
        });
        auto g = Game{driver, b};

        g.move("A2", "A4");
        EXPECT_TRUE(g.move("B4", "A3")); // en passant
        EXPECT_EQ(Empty(), g.current()["A4"]); // should capture A4 pawn.
        EXPECT_EQ(bpawn, g.current()["A3"]);
    }

    TEST_F(MovePawnFixture, en_passant_right)
    {
        auto b = Board::with_pieces({
            {"C2", wpawn},
            {"B4", bpawn}
        });
        auto g = Game{driver, b};

        g.move("C2", "C4");
        EXPECT_TRUE(g.move("B4", "C3")); // en passant
        EXPECT_EQ(Empty(), g.current()["C4"]); // should capture A4 pawn.
        EXPECT_EQ(bpawn, g.current()["C3"]);
    }

    TEST_F(MovePawnFixture, cant_en_passant_if_move_inbetween)
    {
        auto b = Board::with_pieces({
            {"A2", wpawn},
            {"B4", bpawn},
            {"H2", wpawn},
            {"H7", bpawn}
        });

        auto g = Game{driver, b};

        g.move("A2", "A4");

        // Random pawn movements.
        g.move("H7", "H6");
        g.move("H2", "H3");

        EXPECT_FALSE(g.move("B4", "A3")); // try en passant
    }

    TEST_F(MovePawnFixture, cant_en_passant_if_pawn_did_not_move_two)
    {
        auto b = Board::with_pieces({
                {"A2", wpawn},
                {"B4", bpawn},
                {"H2", wpawn},
                {"H7", bpawn}
        });

        auto g = Game{driver, b};

        g.move("A2", "A3");
        g.move("H7", "H6");
        g.move("A3", "A4");

        EXPECT_FALSE(g.move("B4", "A3")); // try en passant
    }

    TEST_F(MovePawnFixture, cant_jump_two_if_first_square_occupied)
    {
        auto b = Board::with_pieces({
            {"A2", wpawn},
            {"A3", bpawn}
        });

        auto g = Game{driver, b};
        EXPECT_FALSE(g.move("A2", "A4"));
    }

    TEST_F(MovePawnFixture, white_gets_promoted_at_end_of_board)
    {
        EXPECT_CALL(driver, promote(_, _)).WillRepeatedly(Return(Queen(Colour::white)));

        auto g = Game{driver, Board::with_pieces({
            {"A7", wpawn}
        })};

        g.move("A7", "A8");
        EXPECT_EQ(Queen(Colour::white), g.current()["A8"]);
    }

    TEST_F(MovePawnFixture, promoting_piece_to_pawn_causes_exeception)
    {
        EXPECT_CALL(driver, promote(_, _)).WillOnce(Return(Pawn(Colour::white)));

        auto g = Game{driver, Board::with_pieces({
            {"A7", wpawn}
        })};

        EXPECT_THROW(g.move("A7", "A8"), InvalidDriverAction);
    }

    TEST_F(MovePawnFixture, promoting_piece_to_empty_causes_exeception_for_white)
    {
        EXPECT_CALL(driver, promote(_, _)).WillOnce(Return(Empty()));

        auto g = Game{driver, Board::with_pieces({
                {"A7", wpawn}
        })};

        EXPECT_THROW(g.move("A7", "A8"), InvalidDriverAction);
    }

    TEST_F(MovePawnFixture, promoting_piece_to_empty_causes_exeception_for_black)
    {
        EXPECT_CALL(driver, promote(_, _)).WillOnce(Return(Empty()));

        auto g = Game{driver, Board::with_pieces({
                {"A5", wpawn},
                {"A2", bpawn}
        })};

        g.move("A5", "A6");

        EXPECT_THROW(g.move("A2", "A1"), InvalidDriverAction);
    }


    TEST_F(MovePawnFixture, black_gets_promoted_at_start_of_board)
    {
        EXPECT_CALL(driver, promote(_, _)).WillRepeatedly(Return(Queen(Colour::black)));

        auto g = Game{driver, Board::with_pieces({
            {"A2", bpawn},
            {"D2", wpawn}
        })};

        g.move("D2", "D3");
        g.move("A2", "A1");
        EXPECT_EQ(Queen(Colour::black), g.current()["A1"]);
    }
}