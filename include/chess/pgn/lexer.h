#pragma once

#include <chess/pgn/token.h>

#include <iosfwd>
#include <memory>

namespace chess::pgn
{
    struct Lexer
    {
        Lexer(std::istream &);

        std::unique_ptr<Token> next();

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

        std::unique_ptr<Token> consume_tag_open_or_movetext();
        std::unique_ptr<Token> consume_tag_open();
        std::unique_ptr<Token> consume_tag_name();
        std::unique_ptr<Token> consume_tag_value();
        std::unique_ptr<Token> consume_tag_close();
        std::unique_ptr<Token> consume_movetext();
        std::unique_ptr<Token> consume_colour_indicator();
        std::unique_ptr<Token> consume_comment();

        std::unique_ptr<Token> san_from_text(std::string text);
    };
}