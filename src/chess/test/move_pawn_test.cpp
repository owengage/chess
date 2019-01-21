#include <gtest/gtest.h>
#include <chess/game.h>
#include <chess/text/print.h>

namespace chess
{
    namespace
    {
        auto constexpr wpawn = Square{Pawn{Colour::white}};
        auto constexpr bpawn = Square{Pawn{Colour::black}};

        Game with_wpawn_at(Loc loc)
        {
            auto b = Board::blank();
            b[loc] = wpawn;
            return Game{b};
        }
    }

    TEST(move_pawn_test, can_move_white_pawn_forward_one)
    {
        auto g = Game{};
        EXPECT_TRUE(g.move("A2", "A3"));
        EXPECT_EQ(wpawn, g.current()["A3"]);
    }

    TEST(move_pawn_test, cant_move_white_pawn_left_right_down)
    {
        auto g = Game{};
        EXPECT_FALSE(g.move("A2", "B2"));
        EXPECT_FALSE(g.move("B2", "A2"));
        EXPECT_FALSE(g.move("A2", "A1"));
    }

    TEST(move_pawn_test, cant_move_pawn_if_not_players_turn)
    {
        auto g = Game{};
        g.move("A2", "A3");
        EXPECT_FALSE(g.move("A3", "A4"));
    }

    TEST(move_pawn_test, cant_move_beyond_end)
    {
        auto g = with_wpawn_at("A8");
        EXPECT_THROW(g.move("A8", "A9"), LocInvalid);
    }

    TEST(move_pawn_test, cant_move_no_space)
    {
        auto g = Game{};
        EXPECT_FALSE(g.move("A2", "A2"));
    }

    TEST(move_pawn_test, cant_move_no_space_at_board_end)
    {
        auto g = with_wpawn_at("D8");
        EXPECT_FALSE(g.move("D8", "D8"));
    }

    TEST(move_pawn_test, cant_diagonal_to_empty)
    {
        auto g = with_wpawn_at("B1");
        EXPECT_FALSE(g.move("B1", "C2"));
        EXPECT_FALSE(g.move("B1", "A2"));
    }

    TEST(move_pawn_test, can_right_diagonal_to_opposite_piece)
    {
        auto b = Board::blank();
        b["A1"] = wpawn;
        b["B2"] = bpawn;

        auto g = Game{b};
        EXPECT_TRUE(g.move("A1", "B2"));
    }

    TEST(move_pawn_test, can_left_diagonal_to_opposite_piece)
    {
        auto b = Board::blank();
        b["B1"] = wpawn;
        b["A2"] = bpawn;

        auto g = Game{b};
        EXPECT_TRUE(g.move("B1", "A2"));
    }

    TEST(move_pawn_test, cant_capture_forward)
    {
        auto b = Board::blank();
        b["B1"] = wpawn;
        b["B2"] = bpawn;

        auto g = Game{b};
        EXPECT_FALSE(g.move("B1", "B2"));
    }

    TEST(move_pawn_test, cant_double_jump_after_own_first_move)
    {
        auto g = Game{};
        g.move("A2", "A3");
        EXPECT_FALSE(g.move("A5", "A5"));
    }

    TEST(move_pawn_test, can_double_jump_on_own_first_move)
    {
        auto g = Game{};
        EXPECT_TRUE(g.move("A2", "A4"));
    }

    TEST(move_pawn_test, en_passant_left)
    {
        auto b = Board::with_pieces({
            {"A2", wpawn},
            {"B4", bpawn}
        });
        auto g = Game{b};

        g.move("A2", "A4");
        EXPECT_TRUE(g.move("B4", "A3")); // en passant
        EXPECT_EQ(Square{Empty{}}, g.current()["A4"]); // should capture A4 pawn.
        EXPECT_EQ(bpawn, g.current()["A3"]);
    }

    TEST(move_pawn_test, en_passant_right)
    {
        auto b = Board::with_pieces({
            {"C2", wpawn},
            {"B4", bpawn}
        });
        auto g = Game{b};

        g.move("C2", "C4");
        EXPECT_TRUE(g.move("B4", "C3")); // en passant
        EXPECT_EQ(Square{Empty{}}, g.current()["C4"]); // should capture A4 pawn.
        EXPECT_EQ(bpawn, g.current()["C3"]);
    }

    TEST(move_pawn_test, cant_en_passant_if_move_inbetween)
    {
        auto b = Board::with_pieces({
            {"A2", wpawn},
            {"B4", bpawn},
            {"H2", wpawn},
            {"H7", bpawn}
        });

        auto g = Game{b};

        g.move("A2", "A4");

        // Random pawn movements.
        g.move("H7", "H6");
        g.move("H2", "H3");

        EXPECT_FALSE(g.move("B4", "A3")); // try en passant
    }

    TEST(move_pawn_test, cant_jump_two_if_first_square_occupied)
    {
        auto b = Board::with_pieces({
            {"A2", wpawn},
            {"A3", bpawn}
        });

        auto g = Game{b};
        EXPECT_FALSE(g.move("A2", "A4"));
    }
}