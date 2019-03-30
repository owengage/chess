#include <chess/board.h>

#include <chess/pgn/resolve_move.h>
#include <chess/pgn/tokens.h>

#include <gtest/gtest.h>

namespace chess::pgn
{
    namespace
    {
    }

    TEST(resolve_move_test, empty_sanmove_does_not_resolve)
    {
        auto move = SanMove{};
        auto board = Board::standard();

        EXPECT_FALSE(resolve_move(move, board));
    }

    TEST(resolve_move_test, basic_pawn_move_resolves)
    {
        auto sanmove = SanMove{0, 2};
        auto board = Board::standard();
        auto move = resolve_move(sanmove, board);

        ASSERT_TRUE(move);
        EXPECT_EQ("A3", move->dest);
    }
}