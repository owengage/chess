#pragma once

#include <chess/pgn/tokens.h>
#include <chess/pgn/Parser.h>

#include <iosfwd>

namespace chess::pgn
{
    struct Lexer
    {
        Lexer(std::istream &, Parser &);

        bool next();

    private:
        enum class State
        {
            expect_tag_open_or_movetext,
            expect_tag_name,
            expect_tag_value,
            expect_tag_close,
            expect_colour_indicator,
            expect_movetext,
            error,
            done,
        };

        State m_state;
        std::istream & m_stream;
        Parser & m_parser;

        bool consume_tag_open_or_movetext();
        bool consume_tag_open();
        bool consume_tag_name();
        bool consume_tag_value();
        bool consume_tag_close();
        bool consume_movetext();
        bool consume_colour_indicator();
        bool consume_comment();
        bool consume_dollar_indicator();
        bool consume_open_alternative();
        bool consume_close_alternative();
        bool san_from_text(std::string text);
    };
}