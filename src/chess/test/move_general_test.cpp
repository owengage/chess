#include <gtest/gtest.h>
#include <chess/game.h>

#include "player_fixture.h"

namespace chess
{
    namespace
    {
        auto constexpr wknight = Square{Knight{Colour::white}};
        auto constexpr wqueen = Square{Queen{Colour::white}};
        auto constexpr wking = Square{King{Colour::white}};
        auto constexpr wpawn = Square{Pawn{Colour::white}};
        auto constexpr wrook = Square{Rook{Colour::white}};
        auto constexpr bknight = Square{Knight{Colour::black}};
        auto constexpr bking = Square{King{Colour::black}};
        auto constexpr brook = Square{Rook{Colour::black}};
        auto constexpr bpawn = Square{Pawn{Colour::black}};
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

    TEST_F(MoveGeneralFixture, cant_make_move_if_leaves_player_in_check)
    {
        auto b = Board::with_pieces({
            {"C4", wknight},
            {"C3", wking},
            {"C5", brook}
        });
        auto g = Game{p1, p2, b};

        EXPECT_FALSE(g.move("C4", "D6"));
    }

    TEST_F(MoveGeneralFixture, cant_move_king_into_direct_danger)
    {
        auto b = Board::with_pieces({
                {"C3", wking},
                {"D8", brook}
        });
        auto g = Game{p1, p2, b};

        EXPECT_FALSE(g.move("C3", "D3"));
    }

    TEST_F(MoveGeneralFixture, can_move_king_out_check)
    {
        auto b = Board::with_pieces({
                {"D1", wking},
                {"D8", brook}
        });
        auto g = Game{p1, p2, b};

        EXPECT_TRUE(g.move("D1", "C1"));
    }

    TEST_F(MoveGeneralFixture, can_king_side_castle)
    {
        auto b = Board::with_pieces({
                {"A1", wrook},
                {"D1", wking}
        });
        auto g = Game{p1, p2, b};

        EXPECT_TRUE(g.move("D1", "B1"));
        EXPECT_EQ(wking, g.current()["B1"]);
        EXPECT_EQ(wrook, g.current()["C1"]);
    }

    TEST_F(MoveGeneralFixture, cant_king_side_castle_while_king_in_check)
    {
        auto b = Board::with_pieces({
                {"A1", wrook},
                {"D1", wking},
                {"D8", brook}
        });
        auto g = Game{p1, p2, b};

        EXPECT_FALSE(g.move("D1", "B1"));
    }

    TEST_F(MoveGeneralFixture, cant_king_side_castle_through_targeted_square)
    {
        auto b = Board::with_pieces({
                {"A1", wrook},
                {"D1", wking},
                {"C8", brook} // can attack king 'through' castling
        });
        auto g = Game{p1, p2, b};

        EXPECT_FALSE(g.move("D1", "B1"));
    }

    TEST_F(MoveGeneralFixture, cant_king_side_castle_if_dest_sq_checked)
    {
        auto b = Board::with_pieces({
                {"A1", wrook},
                {"D1", wking},
                {"B8", brook} // can attack king 'through' castling
        });
        auto g = Game{p1, p2, b};

        EXPECT_FALSE(g.move("D1", "B1"));
    }


    TEST_F(MoveGeneralFixture, cant_king_side_castle_if_rook_has_moved)
    {
        auto b = Board::with_pieces({
                {"F7", bpawn},
                {"A1", wrook},
                {"D1", wking},
        });
        auto g = Game{p1, p2, b};

        EXPECT_TRUE(g.move("A1", "A2")); // move rook
        EXPECT_TRUE(g.move("F7", "F6")); // move pawn to get back to white
        EXPECT_TRUE(g.move("A2", "A1")); // move rook back
        EXPECT_TRUE(g.move("F6", "F5")); // pawn again

        EXPECT_FALSE(g.move("D1", "B1"));
    }

    TEST_F(MoveGeneralFixture, cant_king_side_castle_if_king_has_moved)
    {
        auto b = Board::with_pieces({
                {"F7", bpawn},
                {"A1", wrook},
                {"D1", wking},
        });
        auto g = Game{p1, p2, b};

        EXPECT_TRUE(g.move("D1", "D2")); // move king
        EXPECT_TRUE(g.move("F7", "F6")); // move pawn to get back to white
        EXPECT_TRUE(g.move("D2", "D1")); // move king back
        EXPECT_TRUE(g.move("F6", "F5")); // pawn again

        EXPECT_FALSE(g.move("D1", "B1"));
    }

    TEST_F(MoveGeneralFixture, can_queen_side_castle)
    {
        auto b = Board::with_pieces({
                {"H1", wrook},
                {"D1", wking},
        });
        auto g = Game{p1, p2, b};

        EXPECT_TRUE(g.move("D1", "G1"));
        EXPECT_EQ(wking, g.current()["G1"]);
        EXPECT_EQ(wrook, g.current()["F1"]);
    }

    TEST_F(MoveGeneralFixture, cant_queen_side_castle_with_piece_in_way)
    {
        auto b = Board::with_pieces({
                {"H1", wrook},
                {"D1", wking},
                {"E1", wqueen},
        });
        auto g = Game{p1, p2, b};

        EXPECT_FALSE(g.move("D1", "G1"));
    }
    TEST_F(MoveGeneralFixture, cant_king_side_castle_with_piece_in_way)
    {
        auto b = Board::with_pieces({
                {"A1", wrook},
                {"D1", wking},
                {"B1", wqueen},
        });
        auto g = Game{p1, p2, b};

        EXPECT_FALSE(g.move("D1", "B1"));
    }

    // TODO: Weird checks if king start position is not standard...
}