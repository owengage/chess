#include <chess/pgn/lexer.h>
#include <chess/pgn/parser.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <sstream>

using testing::InSequence;
using testing::A;
using testing::An;
using testing::AllOf;
using testing::Matcher;
using testing::Field;

/**
 * For specification used see
 *      http://www.saremba.de/chessgml/standards/pgn/pgn-complete.htm
 */

namespace chess::pgn
{
    namespace
    {
        struct MockParser : Parser
        {
            MOCK_METHOD1(visit, void(TagPairOpen const&));
            MOCK_METHOD1(visit, void(TagPairClose const&));
            MOCK_METHOD1(visit, void(TagPairName const&));
            MOCK_METHOD1(visit, void(TagPairValue const&));
            MOCK_METHOD1(visit, void(MoveNumber const&));
            MOCK_METHOD1(visit, void(ColourIndicator const&));
            MOCK_METHOD1(visit, void(SanMove const&));
            MOCK_METHOD1(visit, void(SyntaxError const&));
            MOCK_METHOD1(visit, void(TerminationMarker const&));
        };

        int feed(Parser & parser, Lexer & lexer)
        {
            int count = 0;
            while (auto token = lexer.next())
            {
                token->accept(parser);
                count++;
            }

            return count;
        }

        struct PrintParser : Parser
        {
            void visit(TagPairOpen const& token)
            {
                std::cout << "[";
            }

            void visit(TagPairClose const& token)
            {
                std::cout << "]\n";
            }

            void visit(TagPairName const& token)
            {
                std::cout << token.name << ' ';
            }

            void visit(TagPairValue const& token)
            {
                std::cout << '"' << token.value << "\"";
            }

            void visit(MoveNumber const& token)
            {
                std::cout << "Move " << token.number << ' ';
            }

            void visit(ColourIndicator const& token)
            {
                std::cout << (token.colour == Colour::white ? 'W' : 'B') << '\n';
            }

            void visit(SanMove const& token)
            {
                std::cout << *token.dest_x << ", " << *token.dest_y << '\n';
            }

            void visit(SyntaxError const& token)
            {
                std::cout << "SyntaxError";
            }

            void visit(TerminationMarker const& token)
            {
                std::cout << "End " << static_cast<int>(token.type) << '\n';
            }
        };
    }

    MATCHER_P(tag_name_helper, name, "")
    {
        return arg.name == name;
    }

    auto tag_name(std::string name)
    {
        return Matcher<TagPairName const&>(tag_name_helper(name));
    }

    MATCHER_P(tag_value_helper, value, "")
    {
        return arg.value == value;
    }

    auto tag_value(std::string value)
    {
        return Matcher<TagPairValue const&>(tag_value_helper(value));
    }

    MATCHER_P(move_number_helper, number, "")
    {
        return arg.number == number;
    }

    auto move_number(int number)
    {
        return Matcher<MoveNumber const&>(move_number_helper(number));
    }

    MATCHER_P(colour_indicator_helper, colour, "")
    {
        return arg.colour == colour;
    }

    auto colour_indicator(Colour colour)
    {
        return Matcher<ColourIndicator const&>(colour_indicator_helper(colour));
    }

    MATCHER_P(termination_helper, type, "")
    {
        return arg.type == type;
    }

    auto termination(TerminationMarker::Type type)
    {
        return Matcher<TerminationMarker const&>(termination_helper(type));
    }

    TEST(lexer_test, empty_string_produces_no_tokens)
    {
        auto stream = std::istringstream{""};
        auto lexer = Lexer{stream};
        EXPECT_EQ(nullptr, lexer.next());
    }

    TEST(lexer_test, open_tagpair_produces_tagpair_open_token)
    {
        auto stream = std::istringstream{"["};
        auto lexer = Lexer{stream};
        auto token = lexer.next();

        ASSERT_TRUE(token);
        EXPECT_TRUE(dynamic_cast<TagPairOpen*>(token.get()));
    }

    TEST(lexer_test, tag_pair_produces_tokens)
    {
        auto stream = std::istringstream{R"([name "value"])"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        InSequence dummy;
        EXPECT_CALL(parser, visit(An<TagPairOpen const&>()));
        EXPECT_CALL(parser, visit(tag_name("name")));
        EXPECT_CALL(parser, visit(tag_value("value")));
        EXPECT_CALL(parser, visit(An<TagPairClose const&>()));

        feed(parser, lexer);
    }

    TEST(lexer_test, whitespace_in_tag_pair_ignored)
    {
        auto stream = std::istringstream{R"(  [  name    "value" ]  )"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        InSequence dummy;
        EXPECT_CALL(parser, visit(An<TagPairOpen const&>()));
        EXPECT_CALL(parser, visit(tag_name("name")));
        EXPECT_CALL(parser, visit(tag_value("value")));
        EXPECT_CALL(parser, visit(An<TagPairClose const&>()));

        feed(parser, lexer);
    }

    TEST(lexer_test, multiple_tag_pairs_parsed_in_order)
    {
        auto stream = std::istringstream{R"(
            [name1 "value1"]
            [name2 "value2"])"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        InSequence dummy;
        EXPECT_CALL(parser, visit(An<TagPairOpen const&>()));
        EXPECT_CALL(parser, visit(tag_name("name1")));
        EXPECT_CALL(parser, visit(tag_value("value1")));
        EXPECT_CALL(parser, visit(An<TagPairClose const&>()));
        EXPECT_CALL(parser, visit(An<TagPairOpen const&>()));
        EXPECT_CALL(parser, visit(tag_name("name2")));
        EXPECT_CALL(parser, visit(tag_value("value2")));
        EXPECT_CALL(parser, visit(An<TagPairClose const&>()));

        feed(parser, lexer);
    }

    TEST(lexer_test, tag_value_with_space_handled)
    {
        auto stream = std::istringstream{R"([name "some value"])"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(tag_value("some value")));

        feed(parser, lexer);
    }

    TEST(lexer_test, tag_value_with_literal_quote_is_decoded)
    {
        auto stream = std::istringstream{R"([name "some \"value"])"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(tag_value("some \"value")));

        feed(parser, lexer);
    }

    TEST(lexer_test, tag_value_with_literal_backslash_is_decoded)
    {
        auto stream = std::istringstream{R"([name "some \\value"])"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(tag_value("some \\value")));

        feed(parser, lexer);
    }

    TEST(lexer_test, tag_value_with_broken_escape_should_not_lex)
    {
        auto stream = std::istringstream{R"([name "some \a value"])"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(A<TagPairValue const&>())).Times(0);

        feed(parser, lexer);
    }

    TEST(lexer_test, tag_value_with_escape_at_end_of_value_should_not_lex)
    {
        auto stream = std::istringstream{R"([name "some value\"])"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(A<TagPairValue const&>())).Times(0);

        feed(parser, lexer);
    }

    TEST(lexer_test, tag_value_without_quote_mark_start_should_not_lex)
    {
        auto stream = std::istringstream{R"([name some value"])"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(A<TagPairValue const&>())).Times(0);

        feed(parser, lexer);
    }

    TEST(lexer_test, tag_name_can_include_underscores)
    {
        auto stream = std::istringstream{R"([longer__name "some value"])"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(tag_name("longer__name")));

        feed(parser, lexer);
    }

    TEST(lexer_test, move_number_should_lex)
    {
        auto stream = std::istringstream{R"(1.)"};
        auto lexer = Lexer{stream};
        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(move_number(1)));
        feed(parser, lexer);
    }

    TEST(lexer_test, long_move_number)
    {
        auto stream = std::istringstream{R"(123.)"};
        auto lexer = Lexer{stream};
        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(move_number(123)));
        feed(parser, lexer);
    }

    TEST(lexer_test, white_move_indicator)
    {
        auto stream = std::istringstream{R"(123.)"};
        auto lexer = Lexer{stream};
        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        feed(parser, lexer);
    }

    TEST(lexer_test, black_move_indicator)
    {
        auto stream = std::istringstream{R"(123...)"};
        auto lexer = Lexer{stream};
        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(colour_indicator(Colour::black)));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_for_basic_pawn_movement)
    {
        auto stream = std::istringstream{R"(1. a3)"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::src_x, std::nullopt),
                Field(&SanMove::src_y, std::nullopt),
                Field(&SanMove::type, SquareType::pawn),
                Field(&SanMove::capture, false)
        ))));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_for_basic_knight_movement)
    {
        auto stream = std::istringstream{R"(1. Na3)"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::src_x, std::nullopt),
                Field(&SanMove::src_y, std::nullopt),
                Field(&SanMove::type, SquareType::knight),
                Field(&SanMove::capture, false)
        ))));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_disambiguation_file)
    {
        auto stream = std::istringstream{R"(1. Nba3)"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::src_x, 1),
                Field(&SanMove::src_y, std::nullopt),
                Field(&SanMove::type, SquareType::knight),
                Field(&SanMove::capture, false)
        ))));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_disambiguation_rank)
    {
        auto stream = std::istringstream{R"(1. N4a3)"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::src_x, std::nullopt),
                Field(&SanMove::src_y, 3),
                Field(&SanMove::type, SquareType::knight),
                Field(&SanMove::capture, false)
        ))));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_disambiguation_file_and_rank)
    {
        auto stream = std::istringstream{R"(1. Nc4a3)"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::src_x, 2),
                Field(&SanMove::src_y, 3),
                Field(&SanMove::type, SquareType::knight),
                Field(&SanMove::capture, false)
        ))));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_capture)
    {
        auto stream = std::istringstream{R"(1. Nxa3)"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::src_x, std::nullopt),
                Field(&SanMove::src_y, std::nullopt),
                Field(&SanMove::type, SquareType::knight),
                Field(&SanMove::capture, true)
        ))));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_capture_and_src_square)
    {
        auto stream = std::istringstream{R"(1. Nc4xa3)"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::src_x, 2),
                Field(&SanMove::src_y, 3),
                Field(&SanMove::type, SquareType::knight),
                Field(&SanMove::capture, true)
        ))));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_causing_check)
    {
        auto stream = std::istringstream{R"(1. a3+)"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::check, true)
        ))));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_causing_checkmate)
    {
        auto stream = std::istringstream{R"(1. a3#)"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::checkmate, true)
        ))));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_promotion)
    {
        auto stream = std::istringstream{R"(1. g8=Q+)"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 6),
                Field(&SanMove::dest_y, 7),
                Field(&SanMove::check, true),
                Field(&SanMove::promotion, SquareType::queen)
        ))));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_king_side_castle)
    {
        auto stream = std::istringstream{R"(1. O-O a6)"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        InSequence dummy;
        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, std::nullopt),
                Field(&SanMove::dest_y, std::nullopt),
                Field(&SanMove::src_x, std::nullopt),
                Field(&SanMove::src_y, std::nullopt),
                Field(&SanMove::king_side_castle, true)
        ))));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));

        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_queen_side_castle)
    {
        auto stream = std::istringstream{R"(1. O-O-O a6)"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        InSequence dummy;
        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, std::nullopt),
                Field(&SanMove::dest_y, std::nullopt),
                Field(&SanMove::src_x, std::nullopt),
                Field(&SanMove::src_y, std::nullopt),
                Field(&SanMove::queen_side_castle, true)
        ))));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_several_moves_produces_tokens)
    {
        auto stream = std::istringstream{R"(1. g8=Q+ a6 2. b2 a5)"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        EXPECT_CALL(parser, visit(move_number(2)));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));

        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_black_first_doesnt_expect_two_moves)
    {
        auto stream = std::istringstream{R"(1... a6 2. b2 b5)"};
        auto lexer = Lexer{stream};

        auto parser = MockParser{};
        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::black)));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        EXPECT_CALL(parser, visit(move_number(2)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));

        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_invalid_symbol_fails)
    {
        auto stream = std::istringstream{R"(1. i1)"};
        auto lexer = Lexer{stream};
        auto parser = MockParser{};

        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        EXPECT_CALL(parser, visit(A<SyntaxError const&>()));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_bad_file_at_end_causes_syntax_error)
    {
        auto stream = std::istringstream{R"(1. a3d)"};
        auto lexer = Lexer{stream};
        auto parser = MockParser{};

        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        EXPECT_CALL(parser, visit(A<SyntaxError const&>()));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_bad_dest_rank_capture_gives_syntax_error)
    {
        auto stream = std::istringstream{R"(1. x33)"};
        auto lexer = Lexer{stream};
        auto parser = MockParser{};

        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        EXPECT_CALL(parser, visit(A<SyntaxError const&>()));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_bad_dest_rank_gives_syntax_error)
    {
        auto stream = std::istringstream{R"(1. 33)"};
        auto lexer = Lexer{stream};
        auto parser = MockParser{};

        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        EXPECT_CALL(parser, visit(move_number(33)));
        EXPECT_CALL(parser, visit(A<SyntaxError const&>()));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_comment_should_parse)
    {
        auto stream = std::istringstream{R"(1. a2 {Something boring} a6)"};
        auto lexer = Lexer{stream};
        auto parser = MockParser{};

        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_inline_unterminated_comment_should_have_syntax_error)
    {
        auto stream = std::istringstream{R"(1. a2 {Something bo)"};
        auto lexer = Lexer{stream};
        auto parser = MockParser{};

        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        EXPECT_CALL(parser, visit(A<SyntaxError const&>()));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_white_win)
    {
        auto stream = std::istringstream{R"(1. a2 1-0)"};
        auto lexer = Lexer{stream};
        auto parser = MockParser{};

        EXPECT_CALL(parser, visit(termination(TerminationMarker::Type::white_win)));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_draw)
    {
        auto stream = std::istringstream{R"(1. a2 1/2-1/2)"};
        auto lexer = Lexer{stream};
        auto parser = MockParser{};

        EXPECT_CALL(parser, visit(termination(TerminationMarker::Type::draw)));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_with_in_progress_termination)
    {
        auto stream = std::istringstream{R"(1. a2 *)"};
        auto lexer = Lexer{stream};
        auto parser = MockParser{};

        EXPECT_CALL(parser, visit(termination(TerminationMarker::Type::in_progress)));
        feed(parser, lexer);
    }

    TEST(lexer_test, movetext_wiki_example)
    {
        auto stream = std::istringstream{R"(
            [Event "F/S Return Match"]
            [Site "Belgrade, Serbia JUG"]
            [Date "1992.11.04"]
            [Round "29"]
            [White "Fischer, Robert J."]
            [Black "Spassky, Boris V."]
            [Result "1/2-1/2"]

            1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 {This opening is called the Ruy Lopez.}
            4. Ba4 Nf6 5. O-O Be7 6. Re1 b5 7. Bb3 d6 8. c3 O-O 9. h3 Nb8 10. d4 Nbd7
            11. c4 c6 12. cxb5 axb5 13. Nc3 Bb7 14. Bg5 b4 15. Nb1 h6 16. Bh4 c5 17. dxe5
            Nxe4 18. Bxe7 Qxe7 19. exd6 Qf6 20. Nbd2 Nxd6 21. Nc4 Nxc4 22. Bxc4 Nb6
            23. Ne5 Rae8 24. Bxf7+ Rxf7 25. Nxf7 Rxe1+ 26. Qxe1 Kxf7 27. Qe3 Qg5 28. Qxg5
            hxg5 29. b3 Ke6 30. a3 Kd6 31. axb4 cxb4 32. Ra5 Nd5 33. f3 Bc8 34. Kf2 Bf5
            35. Ra7 g6 36. Ra6+ Kc5 37. Ke1 Nf4 38. g3 Nxh3 39. Kd2 Kb5 40. Rd6 Kc5 41. Ra6
            Nf2 42. g4 Bd3 43. Re6 1/2-1/2
        )"};

        auto lexer = Lexer{stream};
        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(termination(TerminationMarker::Type::draw)));
        auto count = feed(parser, lexer);
        EXPECT_EQ(43 * 4 + 7 * 4, count); // 43 moves 4 tokens each, 7 tags 4 tokens each.
    }
}