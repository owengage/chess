#include <chess/pgn/move_parser.h>
#include <chess/pgn/lexer.h>

#include <gtest/gtest.h>

namespace chess::pgn
{
    namespace
    {
        int feed(Lexer & lexer)
        {
            int count = 0;
            while (auto token = lexer.next())
            {
                count++;
            }

            return count;
        }
    }

    TEST(move_parser_test, no_moves_should_have_no_next)
    {
        auto stream = std::istringstream{R"([name "value"])"};
        auto parser = MoveParser{stream};
        EXPECT_FALSE(parser.next());
    }

    TEST(move_parser_test, one_move_read)
    {
        auto stream = std::istringstream{R"([name "value"] a3)"};
        auto parser = MoveParser{stream};
        auto move = parser.next();

        ASSERT_TRUE(move);
        EXPECT_EQ(2, move->dest_y);
        EXPECT_EQ(0, move->dest_x);

        // Really we want to split up stuff into lists of SanMoves. Ideally we want to also
        // handle games that start with odd beginnings as well as starting on black.
    }
}