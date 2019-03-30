#include <chess/pgn/lexer.h>
#include <chess/pgn/tokens.h>

#include <istream>
#include <algorithm>

using chess::SquareType;
using chess::pgn::Lexer;
using chess::pgn::SanMove;
using chess::pgn::TerminationMarker;

namespace
{
    void consume_whitespace(std::istream & stream)
    {
        auto ch = stream.get();
        while (isspace(ch))
        {
            ch = stream.get();
        }
        stream.unget();
    }

    std::optional<SquareType> translate_type_symbol(char sym)
    {
        switch (sym)
        {
            case 'R':
                return SquareType::rook;
            case 'B':
                return SquareType::bishop;
            case 'N':
                return SquareType::knight;
            case 'Q':
                return SquareType::queen;
            case 'K':
                return SquareType::king;
            case 'P':
                return SquareType::pawn;
            default:
                return std::nullopt;
        }
    }

    bool is_file(char ch)
    {
        return ch >= 'a' && ch <= 'h';
    }

    bool is_rank(char ch)
    {
        return ch >= '1' && ch <= '8';
    }

    SquareType extract_type(std::string & text)
    {
        if (auto type = translate_type_symbol(text.front()); type)
        {
            text.erase(0, 1);
            return *type;
        }
        else
        {
            return SquareType::pawn;
        }
    }

    std::optional<SquareType> extract_promotion(std::string & text)
    {
        if (auto type = translate_type_symbol(text.back()); type)
        {
            text.pop_back();
            text.pop_back(); // '=' sign
            return type;
        }
        else
        {
            return std::nullopt;
        }
    }

    bool extract_check(std::string & text)
    {
        if (text.back() == '+')
        {
            text.pop_back();
            return true;
        }
        else
        {
            return false;
        }
    }

    bool extract_checkmate(std::string & text)
    {
        if (text.back() == '#')
        {
            text.pop_back();
            return true;
        }
        else
        {
            return false;
        }
    }

    bool handle_castling(SanMove & san, std::string const& text)
    {
        san.king_side_castle = text == "O-O";
        san.queen_side_castle = text == "O-O-O";
        return san.king_side_castle || san.queen_side_castle;
    }

    std::optional<TerminationMarker> get_termination_marker(std::string const& text)
    {
        auto marker = std::make_optional<TerminationMarker>();
        if (text == "1-0")
        {
            marker->type = TerminationMarker::Type::white_win;
        }
        else if (text == "0-1")
        {
            marker->type = TerminationMarker::Type::black_win;
        }
        else if (text == "1/2-1/2")
        {
            marker->type = TerminationMarker::Type::draw;
        }
        else if (text == "*")
        {
            marker->type = TerminationMarker::Type::in_progress;
        }
        else
        {
            marker = std::nullopt;
        }

        return marker;
    }
}

Lexer::Lexer(std::istream & stream, Parser & parser) :
        m_state{State::expect_tag_open_or_movetext},
        m_stream{stream},
        m_parser{parser}
{}

bool Lexer::next()
{
    consume_whitespace(m_stream);

    auto const ch = m_stream.peek();
    if (ch == -1)
    {
        if (m_state == State::expect_colour_indicator)
        {
            m_state = State::error;
            m_parser.visit(SyntaxError{});
            return false;
        }
        m_state = State::done;
    }

    switch (m_state)
    {
        case State::expect_tag_open_or_movetext:
            return consume_tag_open_or_movetext();
        case State::expect_tag_name:
            return consume_tag_name();
        case State::expect_tag_value:
            return consume_tag_value();
        case State::expect_tag_close:
            return consume_tag_close();
        case State::expect_movetext:
            return consume_movetext();
        case State::expect_colour_indicator:
            return consume_colour_indicator();
        case State::error:
        case State::done:
            return false;
    }
}

bool Lexer::san_from_text(std::string text)
{
    auto san = SanMove{};
    san.original_text = text;
    san.check = extract_check(text);
    san.checkmate = extract_checkmate(text);
    san.promotion = extract_promotion(text);

    if (handle_castling(san, text))
    {
        m_parser.visit(san);
        return true;
    }

    if (auto marker = get_termination_marker(text); marker)
    {
        m_parser.visit(*marker);
        m_state = State::expect_tag_open_or_movetext;
        return true;
    }

    san.type = extract_type(text);

    // Destination is always present
    if (is_rank(text.back()))
    {
        san.dest_y = text.back() - '1';
        text.pop_back();
    }
    else
    {
        m_state = State::error;
        m_parser.visit(SyntaxError{});
        return false;
    }

    if (is_file(text.back()))
    {
        san.dest_x = text.back() - 'a';
        text.pop_back();
    }
    else
    {
        m_state = State::error;
        m_parser.visit(SyntaxError{});
        return false;
    }

    if (text.empty())
    {
        m_parser.visit(san);
        return true;
    }

    if (text.back() == 'x')
    {
        san.capture = true;
        text.pop_back();
    }

    if (text.empty())
    {
        m_parser.visit(san);
        return true;
    }

    if (is_file(text.back()))
    {
        san.src_x = text.back() - 'a';
        text.pop_back();
    }
    else if (is_rank(text.back()))
    {
        san.src_y = text.back() - '1';
        text.pop_back();
    }

    if (text.empty())
    {
        m_parser.visit(san);
        return true;
    }

    // If we've got this far then the previous character should have been a rank, and we now
    // expect a file.
    if (is_file(text.back()))
    {
        san.src_x = text.back() - 'a';
        text.pop_back();
    }

    m_parser.visit(san);
    return true;
}

bool Lexer::consume_tag_open_or_movetext()
{
    auto const ch = m_stream.peek();

    if (ch == '[')
    {
        return consume_tag_open();
    }
    else
    {
        return consume_movetext();
    }
}

bool Lexer::consume_tag_open()
{
    auto const ch = m_stream.get();

    if (ch == '[')
    {
        m_state = State::expect_tag_name;
        m_parser.visit(TagPairOpen{});
        return true;
    }
    else
    {
        m_state = State::error;
        m_parser.visit(SyntaxError{});
        return false;
    }
}

bool Lexer::consume_movetext()
{
    auto ch = m_stream.peek();
    auto str = std::string{};

    if (ch == '$')
    {
        return consume_dollar_indicator();
    }

    if (ch == '{')
    {
        return consume_comment();
    }

    if (ch == '(')
    {
        return consume_open_alternative();
    }

    if (ch == ')')
    {
        return consume_close_alternative();
    }

    ch = m_stream.get();

    while (!isspace(ch) && ch != '.' && ch != ')' && ch != '(' && ch != -1)
    {
        str += static_cast<char>(ch);
        ch = m_stream.get();
    }
    m_stream.unget();

    auto file_it = std::find_if(begin(str), end(str), [](char c)
    {
        return is_file(c) || c == 'O' || c == '-' || c == '*';
    });

    bool is_movesan = file_it != end(str);

    if (is_movesan)
    {
        m_state = State::expect_movetext;
        return san_from_text(str);
    }
    else
    {
        int number;

        try
        {
            number = std::stoi(str);
        }
        catch (std::exception const& e)
        {
            m_state = State::error;
            m_parser.visit(SyntaxError{});
            return false;
        }

        m_state = State::expect_colour_indicator;
        m_parser.visit(MoveNumber{number});
        return true;
    }
}

bool Lexer::consume_tag_name()
{
    auto ch = m_stream.get();
    auto name = std::string{};

    while (isalnum(ch) || ch == '_')
    {
        name += static_cast<char>(ch);
        ch = m_stream.get();
    }

    m_state = State::expect_tag_value;
    m_parser.visit(TagPairName{std::move(name)});
    return true;
}

bool Lexer::consume_comment()
{
    auto ch = m_stream.get();

    while (ch != '}')
    {
        if (ch == -1)
        {
            m_state = State::error;
            m_parser.visit(SyntaxError{});
            return false;
        }
        ch = m_stream.get();
    }

    return true;
}

bool Lexer::consume_dollar_indicator()
{
    auto ch = m_stream.get();

    if (ch != '$')
    {
        m_state = State::error;
        m_parser.visit(SyntaxError{});
        return false;
    }

    ch = m_stream.get();

    while (isspace(ch) && ch != -1)
    {
        ch = m_stream.get();
    }

    return true;
}

bool Lexer::consume_open_alternative()
{
    auto ch = m_stream.get();

    if (ch == '(')
    {
        m_parser.visit(AlternativeOpen{});
        return true;
    }
    else
    {
        m_state = State::error;
        m_parser.visit(SyntaxError{});
        return false;
    }
}

bool Lexer::consume_close_alternative()
{
    auto ch = m_stream.get();

    if (ch == ')')
    {
        m_parser.visit(AlternativeClose{});
        return true;
    }
    else
    {
        m_state = State::error;
        m_parser.visit(SyntaxError{});
        return false;
    }
}


bool Lexer::consume_colour_indicator()
{
    auto ch = m_stream.get();
    auto count = 0;

    while (ch == '.')
    {
        count++;
        ch = m_stream.get();
    }
    m_stream.unget();

    Colour colour;

    if (count == 1)
    {
        colour = Colour::white;
    }
    else if (count == 3)
    {
        colour = Colour::black;
    }
    else
    {
        m_state = State::error;
        m_parser.visit(SyntaxError{});
        return false;
    }

    m_state = State::expect_movetext;
    m_parser.visit(ColourIndicator{colour});
    return true;
}

bool Lexer::consume_tag_value()
{
    auto ch = m_stream.get();
    auto value = std::string{};

    if (ch != '"')
    {
        m_state = State::error;
        m_parser.visit(SyntaxError{});
        return false;
    }

    ch = m_stream.get();

    while (ch != '"' && ch != -1 && ch != ']')
    {
        if (ch == '\\')
        {
            ch = m_stream.get();
            if (ch != '"' && ch != '\\')
            {
                m_state = State::error;
                m_parser.visit(SyntaxError{});
                return false;
            }
        }

        value += static_cast<char>(ch);
        ch = m_stream.get();
    }

    if (ch == ']')
    {
        m_state = State::error;
        m_parser.visit(SyntaxError{});
        return false;
    }

    m_state = State::expect_tag_close;
    m_parser.visit(TagPairValue{value});
    return true;
}

bool Lexer::consume_tag_close()
{
    auto const ch = m_stream.get();

    if (ch == ']')
    {
        m_state = State::expect_tag_open_or_movetext;
        m_parser.visit(TagPairClose{});
        return true;
    }

    m_state = State::error;
    m_parser.visit(SyntaxError{});
    return false;
}