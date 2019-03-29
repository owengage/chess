#include <chess/pgn/move_parser.h>
#include <chess/pgn/lexer.h>

#include <fstream>

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

        struct MoveParserFixture : testing::Test
        {
            MoveParser parser_for(std::string text)
            {
                // Stream needs to survive for the whole test.
                stream = std::istringstream{text};
                return MoveParser{stream};
            }

            std::istringstream stream;
        };
    }

    TEST_F(MoveParserFixture, no_text_should_return_nullopt)
    {
        auto parser = parser_for(R"()");
        EXPECT_FALSE(parser.next_game());
    }

    TEST_F(MoveParserFixture, game_without_termination_should_throw_incomplete_game_error)
    {
        auto parser = parser_for(R"([name "value"])");
        EXPECT_THROW(parser.next_game(), IncompleteGameError);
    }

    TEST_F(MoveParserFixture, one_move_in_progress_game_should_be_returned)
    {
        auto parser = parser_for(R"([name "value"] a3 *)");
        auto game = parser.next_game();

        ASSERT_TRUE(game);
        EXPECT_EQ(1, game->size());
        EXPECT_FALSE(parser.next_game());
    }

    TEST_F(MoveParserFixture, games_with_missing_tags_should_be_recognised)
    {
        auto parser = parser_for(R"(a3 * a4 g6 1-0)");
        auto game = parser.next_game();
        ASSERT_TRUE(game);
        EXPECT_EQ(1, game->size());

        game = parser.next_game();
        ASSERT_TRUE(game);
        EXPECT_EQ(2, game->size());
        EXPECT_FALSE(parser.next_game());
    }

    TEST_F(MoveParserFixture, game_with_alternative_doesnt_count_moves)
    {
        auto parser = parser_for(R"(a3 (a4) 1-0)");
        auto game = parser.next_game();
        ASSERT_TRUE(game);
        EXPECT_EQ(1, game->size());
    }

    TEST_F(MoveParserFixture, game_with_alternative_termination_doesnt_count_as_game)
    {
        auto parser = parser_for(R"(a3 (a4 g5 0-1) 1-0)");
        auto game = parser.next_game();
        ASSERT_TRUE(game);
        EXPECT_EQ(1, game->size());
        EXPECT_FALSE(parser.next_game());
    }

    TEST_F(MoveParserFixture, game_with_deep_alternative_termination_doesnt_count_as_game)
    {
        auto parser = parser_for(R"(a3 (a4 g5 (a6 1-0)) 1-0)");
        auto game = parser.next_game();
        ASSERT_TRUE(game);
        EXPECT_EQ(1, game->size());
        EXPECT_FALSE(parser.next_game());
    }

    // TODO: Handle black moving first
    // TODO: Handle FEN/EDN tag (ie non-standard start)
}