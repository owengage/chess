#include <chess/Suggester.h>
#include <chess/Board.h>

#include <gtest/gtest.h>

namespace chess
{
    TEST(suggester_test, board_with_one_move_suggests_that_move)
    {
        auto board = Board::with_pieces({
                {"A2", Pawn(Colour::white)},
                {"A4", Pawn(Colour::black)}
        });

        auto suggester = Suggester{board, evaluate_with_summation};
        auto move = suggester.suggest();
        EXPECT_EQ(Pawn(Colour::white), move.result["A3"]);
    }

    TEST(suggester_test, basic_evaluator)
    {
        auto board = Board::standard();
        auto suggester = Suggester{board, evaluate_with_summation};
        auto move = suggester.suggest();
    }
}