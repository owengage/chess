#include <chess/pgn/Lexer.h>
#include <chess/pgn/Parser.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <sstream>
#include <fstream>

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
            MOCK_METHOD1(visit, void(AlternativeOpen const&));
            MOCK_METHOD1(visit, void(AlternativeClose const&));
            MOCK_METHOD1(visit, void(SyntaxError const&));
            MOCK_METHOD1(visit, void(TerminationMarker const&));
        };

        int feed(Lexer & lexer)
        {
            int count = 0;
            while (auto token = lexer.next())
            {
                count++;
            }

            return count;
        }

        struct PrintParser : Parser
        {
            void visit(TagPairOpen const& token) override
            {
                std::cout << "[";
            }

            void visit(TagPairClose const& token) override
            {
                std::cout << "]\n";
            }

            void visit(TagPairName const& token) override
            {
                std::cout << token.name << ' ';
            }

            void visit(TagPairValue const& token) override
            {
                std::cout << '"' << token.value << "\"";
            }

            void visit(MoveNumber const& token) override
            {
                std::cout << "Move " << token.number << ' ';
            }

            void visit(ColourIndicator const& token) override
            {
                std::cout << (token.colour == Colour::white ? 'W' : 'B') << '\n';
            }

            void visit(SanMove const& token) override
            {
                std::cout << *token.dest_x << ", " << *token.dest_y << '\n';
            }

            void visit(SyntaxError const& token) override
            {
                std::cout << "SyntaxError";
            }

            void visit(TerminationMarker const& token) override
            {
                std::cout << "End " << static_cast<int>(token.type) << '\n';
            }

            void visit(AlternativeOpen const &open) override
            {
                std::cout << "AltOpen";

            }

            void visit(AlternativeClose const &alternativeClose) override
            {
                std::cout << "AltClose";
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
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};
        EXPECT_EQ(false, lexer.next());
    }

    TEST(lexer_test, open_tagpair_produces_tagpair_open_token)
    {
        auto stream = std::istringstream{"["};
        auto parser = MockParser{};
        EXPECT_CALL(parser, visit(An<TagPairOpen const&>()));

        auto lexer = Lexer{stream, parser};
        auto token = lexer.next();

        ASSERT_TRUE(token);
    }

    TEST(lexer_test, tag_pair_produces_tokens)
    {
        auto stream = std::istringstream{R"([name "value"])"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        InSequence dummy;
        EXPECT_CALL(parser, visit(An<TagPairOpen const&>()));
        EXPECT_CALL(parser, visit(tag_name("name")));
        EXPECT_CALL(parser, visit(tag_value("value")));
        EXPECT_CALL(parser, visit(An<TagPairClose const&>()));

        feed(lexer);
    }

    TEST(lexer_test, whitespace_in_tag_pair_ignored)
    {
        auto stream = std::istringstream{R"(  [  name    "value" ]  )"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        InSequence dummy;
        EXPECT_CALL(parser, visit(An<TagPairOpen const&>()));
        EXPECT_CALL(parser, visit(tag_name("name")));
        EXPECT_CALL(parser, visit(tag_value("value")));
        EXPECT_CALL(parser, visit(An<TagPairClose const&>()));

        feed(lexer);
    }

    TEST(lexer_test, multiple_tag_pairs_parsed_in_order)
    {
        auto stream = std::istringstream{R"(
            [name1 "value1"]
            [name2 "value2"])"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        InSequence dummy;
        EXPECT_CALL(parser, visit(An<TagPairOpen const&>()));
        EXPECT_CALL(parser, visit(tag_name("name1")));
        EXPECT_CALL(parser, visit(tag_value("value1")));
        EXPECT_CALL(parser, visit(An<TagPairClose const&>()));
        EXPECT_CALL(parser, visit(An<TagPairOpen const&>()));
        EXPECT_CALL(parser, visit(tag_name("name2")));
        EXPECT_CALL(parser, visit(tag_value("value2")));
        EXPECT_CALL(parser, visit(An<TagPairClose const&>()));

        feed(lexer);
    }

    TEST(lexer_test, tag_value_with_space_handled)
    {
        auto stream = std::istringstream{R"([name "some value"])"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(tag_value("some value")));

        feed(lexer);
    }

    TEST(lexer_test, tag_value_with_literal_quote_is_decoded)
    {
        auto stream = std::istringstream{R"([name "some \"value"])"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(tag_value("some \"value")));

        feed(lexer);
    }

    TEST(lexer_test, tag_value_with_literal_backslash_is_decoded)
    {
        auto stream = std::istringstream{R"([name "some \\value"])"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(tag_value("some \\value")));

        feed(lexer);
    }

    TEST(lexer_test, tag_value_with_broken_escape_should_not_lex)
    {
        auto stream = std::istringstream{R"([name "some \a value"])"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(A<TagPairValue const&>())).Times(0);

        feed(lexer);
    }

    TEST(lexer_test, tag_value_with_escape_at_end_of_value_should_not_lex)
    {
        auto stream = std::istringstream{R"([name "some value\"])"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(A<TagPairValue const&>())).Times(0);

        feed(lexer);
    }

    TEST(lexer_test, tag_value_without_quote_mark_start_should_not_lex)
    {
        auto stream = std::istringstream{R"([name some value"])"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(A<TagPairValue const&>())).Times(0);

        feed(lexer);
    }

    TEST(lexer_test, tag_name_can_include_underscores)
    {
        auto stream = std::istringstream{R"([longer__name "some value"])"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(tag_name("longer__name")));

        feed(lexer);
    }

    TEST(lexer_test, move_number_should_lex)
    {
        auto stream = std::istringstream{R"(1.)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};
        EXPECT_CALL(parser, visit(move_number(1)));
        feed(lexer);
    }

    TEST(lexer_test, long_move_number)
    {
        auto stream = std::istringstream{R"(123.)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};
        EXPECT_CALL(parser, visit(move_number(123)));
        feed(lexer);
    }

    TEST(lexer_test, white_move_indicator)
    {
        auto stream = std::istringstream{R"(123.)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        feed(lexer);
    }

    TEST(lexer_test, black_move_indicator)
    {
        auto stream = std::istringstream{R"(123...)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};
        EXPECT_CALL(parser, visit(colour_indicator(Colour::black)));
        feed(lexer);
    }

    TEST(lexer_test, movetext_for_basic_pawn_movement)
    {
        auto stream = std::istringstream{R"(1. a3)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::src_x, std::nullopt),
                Field(&SanMove::src_y, std::nullopt),
                Field(&SanMove::type, SquareType::pawn),
                Field(&SanMove::capture, false)
        ))));
        feed(lexer);
    }

    TEST(lexer_test, movetext_for_basic_knight_movement)
    {
        auto stream = std::istringstream{R"(1. Na3)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::src_x, std::nullopt),
                Field(&SanMove::src_y, std::nullopt),
                Field(&SanMove::type, SquareType::knight),
                Field(&SanMove::capture, false)
        ))));
        feed(lexer);
    }

    TEST(lexer_test, movetext_with_disambiguation_file)
    {
        auto stream = std::istringstream{R"(1. Nba3)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::src_x, 1),
                Field(&SanMove::src_y, std::nullopt),
                Field(&SanMove::type, SquareType::knight),
                Field(&SanMove::capture, false)
        ))));
        feed(lexer);
    }

    TEST(lexer_test, movetext_with_disambiguation_rank)
    {
        auto stream = std::istringstream{R"(1. N4a3)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::src_x, std::nullopt),
                Field(&SanMove::src_y, 3),
                Field(&SanMove::type, SquareType::knight),
                Field(&SanMove::capture, false)
        ))));
        feed(lexer);
    }

    TEST(lexer_test, movetext_with_disambiguation_file_and_rank)
    {
        auto stream = std::istringstream{R"(1. Nc4a3)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::src_x, 2),
                Field(&SanMove::src_y, 3),
                Field(&SanMove::type, SquareType::knight),
                Field(&SanMove::capture, false)
        ))));
        feed(lexer);
    }

    TEST(lexer_test, movetext_with_capture)
    {
        auto stream = std::istringstream{R"(1. Nxa3)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::src_x, std::nullopt),
                Field(&SanMove::src_y, std::nullopt),
                Field(&SanMove::type, SquareType::knight),
                Field(&SanMove::capture, true)
        ))));
        feed(lexer);
    }

    TEST(lexer_test, movetext_with_capture_and_src_square)
    {
        auto stream = std::istringstream{R"(1. Nc4xa3)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::src_x, 2),
                Field(&SanMove::src_y, 3),
                Field(&SanMove::type, SquareType::knight),
                Field(&SanMove::capture, true)
        ))));
        feed(lexer);
    }

    TEST(lexer_test, movetext_causing_check)
    {
        auto stream = std::istringstream{R"(1. a3+)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::check, true)
        ))));
        feed(lexer);
    }

    TEST(lexer_test, movetext_causing_checkmate)
    {
        auto stream = std::istringstream{R"(1. a3#)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 0),
                Field(&SanMove::dest_y, 2),
                Field(&SanMove::checkmate, true)
        ))));
        feed(lexer);
    }

    TEST(lexer_test, movetext_with_promotion)
    {
        auto stream = std::istringstream{R"(1. g8=Q+)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 6),
                Field(&SanMove::dest_y, 7),
                Field(&SanMove::check, true),
                Field(&SanMove::promotion, SquareType::queen)
        ))));
        feed(lexer);
    }

    TEST(lexer_test, movetext_king_side_castle)
    {
        auto stream = std::istringstream{R"(1. O-O a6)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        InSequence dummy;
        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, std::nullopt),
                Field(&SanMove::dest_y, std::nullopt),
                Field(&SanMove::src_x, std::nullopt),
                Field(&SanMove::src_y, std::nullopt),
                Field(&SanMove::king_side_castle, true)
        ))));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));

        feed(lexer);
    }

    TEST(lexer_test, movetext_queen_side_castle)
    {
        auto stream = std::istringstream{R"(1. O-O-O a6)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        InSequence dummy;
        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, std::nullopt),
                Field(&SanMove::dest_y, std::nullopt),
                Field(&SanMove::src_x, std::nullopt),
                Field(&SanMove::src_y, std::nullopt),
                Field(&SanMove::queen_side_castle, true)
        ))));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        feed(lexer);
    }

    TEST(lexer_test, movetext_with_several_moves_produces_tokens)
    {
        auto stream = std::istringstream{R"(1. g8=Q+ a6 2. b2 a5)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        EXPECT_CALL(parser, visit(move_number(2)));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));

        feed(lexer);
    }

    TEST(lexer_test, movetext_with_black_first_doesnt_expect_two_moves)
    {
        auto stream = std::istringstream{R"(1... a6 2. b2 b5)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::black)));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        EXPECT_CALL(parser, visit(move_number(2)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));

        feed(lexer);
    }

    TEST(lexer_test, movetext_with_invalid_symbol_fails)
    {
        auto stream = std::istringstream{R"(1. i1)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        EXPECT_CALL(parser, visit(A<SyntaxError const&>()));
        feed(lexer);
    }

    TEST(lexer_test, movetext_with_bad_file_at_end_causes_syntax_error)
    {
        auto stream = std::istringstream{R"(1. a3d)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        EXPECT_CALL(parser, visit(A<SyntaxError const&>()));
        feed(lexer);
    }

    TEST(lexer_test, movetext_with_bad_dest_rank_capture_gives_syntax_error)
    {
        auto stream = std::istringstream{R"(1. x33)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        EXPECT_CALL(parser, visit(A<SyntaxError const&>()));
        feed(lexer);
    }

    TEST(lexer_test, movetext_with_bad_dest_rank_gives_syntax_error)
    {
        auto stream = std::istringstream{R"(1. 33)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        EXPECT_CALL(parser, visit(move_number(33)));
        EXPECT_CALL(parser, visit(A<SyntaxError const&>()));
        feed(lexer);
    }

    TEST(lexer_test, movetext_with_comment_should_parse)
    {
        auto stream = std::istringstream{R"(1. a2 {Something boring} a6)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        feed(lexer);
    }

    TEST(lexer_test, movetext_with_inline_unterminated_comment_should_have_syntax_error)
    {
        auto stream = std::istringstream{R"(1. a2 {Something bo)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        EXPECT_CALL(parser, visit(A<SyntaxError const&>()));
        feed(lexer);
    }

    TEST(lexer_test, comment_with_whitespace_at_start_parses)
    {
        auto stream = std::istringstream{R"(1. a2 { Something boring } a6)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(colour_indicator(Colour::white)));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        feed(lexer);
    }

    TEST(lexer_test, movetext_with_white_win)
    {
        auto stream = std::istringstream{R"(1. a2 1-0)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(termination(TerminationMarker::Type::white_win)));
        feed(lexer);
    }

    TEST(lexer_test, movetext_with_draw)
    {
        auto stream = std::istringstream{R"(1. a2 1/2-1/2)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(termination(TerminationMarker::Type::draw)));
        feed(lexer);
    }

    TEST(lexer_test, movetext_with_in_progress_termination)
    {
        auto stream = std::istringstream{R"(1. a2 *)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(termination(TerminationMarker::Type::in_progress)));
        feed(lexer);
    }

    TEST(lexer_test, second_game_should_also_get_lexed)
    {
        auto stream = std::istringstream{R"(1. a3 * 1. e4)"};
        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        InSequence dummy;
        EXPECT_CALL(parser, visit(move_number(1)));
        EXPECT_CALL(parser, visit(termination(TerminationMarker::Type::in_progress)));
        EXPECT_CALL(parser, visit(move_number(1)));

        feed(lexer);
    }

    TEST(lexer_test, movetext_with_castling_that_causes_check)
    {
        auto stream = std::istringstream{R"(
            Be2 O-O-O+
        )"};

        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        InSequence dummy;
        EXPECT_CALL(parser, visit(A<SanMove const&>()));
        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::queen_side_castle, true)
        ))));

        feed(lexer);
    }

    TEST(lexer_test, movetext_with_axb6_bug)
    {
        auto stream = std::istringstream{R"(
            axb6
        )"};

        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(Matcher<SanMove const&>(AllOf(
                Field(&SanMove::dest_x, 1),
                Field(&SanMove::dest_y, 5),
                Field(&SanMove::src_x, 0),
                Field(&SanMove::src_y, std::nullopt),
                Field(&SanMove::capture, true)
        ))));

        feed(lexer);
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

        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(termination(TerminationMarker::Type::draw)));

        auto count = feed(lexer);
        auto expected_count = 43 * 4 + 7 * 4 + 1; // +1 for 'end of stream' iteration of loop
        // 43 moves 4 tokens each, 7 tags 4 tokens each. One comment.
        EXPECT_EQ(expected_count, count);
    }

    TEST(lexer_test, pgn_example_1)
    {
        auto stream = std::istringstream{R"(
            [Event "2.f"][Site "Leningrad"]
            [Date "1974.??.??"][Round "3"][White "Karpov, Anatoly"]
            [Black "Spassky, Boris"][Result "1-0"][ECO "E91"]
            [WhiteElo "2700"][BlackElo "2650"][Annotator "JvR"]
            [PlyCount "109"]
            [EventDate "1974.??.??"]

            1. d4 Nf6 2. c4 g6 3. Nc3 Bg7 4. e4 d6 5. Nf3 O-O 6. Be2 c5
            7. O-O Bg4 $5 { Spassky chooses a sharp opening.} 8. d5 Nbd7
            9. Bg5 a6 10. a4 Qc7 11. Qd2 Rae8 12. h3 Bxf3 13. Bxf3 e6 $5
            14. b3 Kh8 15. Be3 Ng8 16. Be2 e5 $5 17. g4 Qd8
            18. Kg2 Qh4 $5 {Black takes the initiative on the kingside.}
            19. f3 ({ The tactical justification is} 19. Bg5 Bh6)
            19... Bh6 $2 { Tal, Keres and Botvinnik condemn this provocative move} ({and prefer} 19... f5)
            20. g5 Bg7 21. Bf2 Qf4 22. Be3 Qh4 23. Qe1 $1 Qxe1 24. Rfxe1 h6
            25. h4 hxg5 $2 ({A defence line against an attack on the queenside creates}
            25... Ra8 26. Reb1 Rfb8 27. b4 Bf8 28. bxc5 Nxc5) 26. hxg5 Ne7 27. a5 f6
            28. Reb1 fxg5 29. b4 $1 Nf5 $5 30. Bxg5 $1 ({Keres analyses} 30. exf5 e4
            31. Bd2 exf3+ 32. Bxf3 gxf5 { Black has counter-play.})
            30... Nd4 31. bxc5 Nxc5 32. Rb6 Bf6 33. Rh1+ $1 Kg7 34. Bh6+ Kg8
            35. Bxf8 Rxf8 36. Rxd6 Kg7 37. Bd1 Be7 ({Tal mentions} 37... Bd8
            38. Na4 Bc7 39. Nxc5 Bxd6 40. Nxb7 {and 41.c5. White wins.}) 38. Rb6 Bd8
            39. Rb1 Rf7 40. Na4 Nd3 41. Nb6 g5 42. Nc8 Nc5 43. Nd6 Rd7 44. Nf5+ Nxf5
            45. exf5 e4 46. fxe4 Nxe4 47. Ba4 Re7 48. Rbe1 Nc5 49. Rxe7+ Bxe7 50. Bc2 Bd8
            51. Ra1 Kf6 52. d6 Nd7 53. Rb1 Ke5 54. Rd1 Kf4 55. Re1 1-0
        )"};

        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(A<AlternativeOpen const&>())).Times(5);
        EXPECT_CALL(parser, visit(A<AlternativeClose const&>())).Times(5);
        EXPECT_CALL(parser, visit(termination(TerminationMarker::Type::white_win)));

        feed(lexer);
    }

    TEST(lexer_test, pgn_example_2)
    {
        auto stream = std::istringstream{R"(
            [Event "London"]
            [Site "London"]
            [Date "1856.??.??"]
            [EventDate "?"]
            [Round "?"]
            [Result "1-0"]
            [White "Cunningham"]
            [Black "Thomas Wilson Barnes"]
            [ECO "C01"]
            [WhiteElo "?"]
            [BlackElo "?"]
            [PlyCount "115"]

                1.e4 e6 2.d4 d5 3.exd5 exd5 4.Nf3 Nf6 5.Be3 Bd6 6.Bd3 O-O
                7.O-O Be6 8.Ng5 Bg4 9.f3 Bh5 10.Qd2 b6 11.Qf2 Nbd7 12.Nd2 c5
                13.c3 Qc7 14.g4 Bg6 15.Bxg6 hxg6 16.Rac1 Rae8 17.Rfe1 Rxe3
                18.Rxe3 cxd4 19.Rd3 Ne5 20.Rxd4 Bc5 21.Qf1 Nc6 22.Nb3 Nxd4
                23.cxd4 Bxd4+ 24.Nxd4 Qf4 25.Nc6 Qxg5 26.Ne7+ Kh8 27.Re1 Qh4
                28.Re5 Rd8 29.Qd3 Qh3 30.g5 Ng4 31.Re2 d4 32.Rg2 Ne3 33.Rg3
                Qe6 34.Qe4 Nf5 35.Rh3+ Nh6 36.Qxe6 fxe6 37.Kf2 Kh7 38.Ke1 Rd7
                39.Nc6 Rc7 40.Ne5 Rc2 41.Nd3 a5 42.f4 b5 43.Kd1 Rg2 44.Kc1 b4
                45.gxh6 gxh6 46.Nc5 Re2 47.Rd3 e5 48.fxe5 Rxh2 49.Rxd4 Re2
                50.Nd3 g5 51.Kd1 Rg2 52.e6 Kg7 53.Re4 Kf8 54.Ne5 b3 55.axb3
                Rxb2 56.Ng6+ Ke8 57.Rc4 Kd8 58.e7+ 1-0
        )"};

        auto parser = MockParser{};
        auto lexer = Lexer{stream, parser};

        EXPECT_CALL(parser, visit(termination(TerminationMarker::Type::white_win)));

        feed(lexer);
    }

}