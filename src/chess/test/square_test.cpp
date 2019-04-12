#include <chess/Square.h>

#include <gtest/gtest.h>

namespace chess
{
    auto constexpr white = Colour::white;
    auto constexpr black = Colour::black;

    TEST(square_test, should_be_one_byte)
    {
        static_assert(sizeof(Square) == 1);
    }

    TEST(square_test, piece_made_with_colour_should_equal_that_colour)
    {
        EXPECT_EQ(white, Pawn(white).colour());
        EXPECT_EQ(black, Pawn(black).colour());
    }

    TEST(square_test, piece_should_have_correct_type)
    {
        EXPECT_EQ(SquareType::pawn, Pawn(white).type());
        EXPECT_EQ(SquareType::rook, Rook(white).type());
        EXPECT_EQ(SquareType::knight, Knight(white).type());
        EXPECT_EQ(SquareType::bishop, Bishop(white).type());
        EXPECT_EQ(SquareType::queen, Queen(white).type());
        EXPECT_EQ(SquareType::king, King(white).type());
        EXPECT_EQ(SquareType::empty, Empty().type());
    }

    TEST(square_test, moved_should_default_to_false)
    {
        EXPECT_FALSE(Pawn(white).has_moved());
    }

    TEST(square_test, two_pawns_should_compare_equal)
    {
        EXPECT_EQ(Pawn(white), Pawn(white));
    }

    TEST(square_test, two_pawns_one_with_moved_should_compare_equal)
    {
        auto p = Pawn(white);
        p.set_moved();

        EXPECT_TRUE(p.has_moved());
        EXPECT_EQ(Pawn(white), p);
    }

    TEST(square_test, opposite_colours_should_not_be_equal)
    {
        EXPECT_NE(Pawn(white), Pawn(black));
    }

    TEST(square_test, different_types_should_not_be_equal)
    {
        EXPECT_NE(Pawn(white), Rook(white));
    }

    TEST(square_test, setting_type_should_cause_type_to_change)
    {
        auto p = Pawn(white);
        p.set_type(SquareType::rook);
        EXPECT_EQ(SquareType::rook, p.type());
    }
}