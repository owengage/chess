#include <chess/pgn/lexer.h>
#include <chess/pgn/token.h>

#include <istream>
#include <algorithm>

using chess::SquareType;
using chess::pgn::Lexer;
using chess::pgn::Token;
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

    std::unique_ptr<Token> get_termination_marker(std::string const& text)
    {
        auto marker = std::make_unique<TerminationMarker>();
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
            marker = nullptr;
        }

        return marker;
    }
}

Lexer::Lexer(std::istream & stream) : m_state{State::expect_tag_open_or_movetext}, m_stream{stream}
{}

std::unique_ptr<Token> Lexer::next()
{
    consume_whitespace(m_stream);

    auto const ch = m_stream.peek();
    if (ch == -1)
    {
        if (m_state == State::expect_colour_indicator)
        {
            m_state = State::error;
            return std::make_unique<SyntaxError>();
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
            return nullptr;
    }
}

std::unique_ptr<Token> Lexer::san_from_text(std::string text)
{
    auto san = std::make_unique<SanMove>();

    if (handle_castling(*san, text))
    {
        return san;
    }

    if (auto marker = get_termination_marker(text); marker)
    {
        m_state = State::done;
        return marker;
    }

    san->type = extract_type(text);
    san->check = extract_check(text);
    san->checkmate = extract_checkmate(text);
    san->promotion = extract_promotion(text);

    // Destination is always present
    if (is_rank(text.back()))
    {
        san->dest_y = text.back() - '1';
        text.pop_back();
    }
    else
    {
        m_state = State::error;
        return std::make_unique<SyntaxError>();
    }

    if (is_file(text.back()))
    {
        san->dest_x = text.back() - 'a';
        text.pop_back();
    }
    else
    {
        m_state = State::error;
        return std::make_unique<SyntaxError>();
    }

    if (text.empty())
    {
        return san;
    }

    if (text.back() == 'x')
    {
        san->capture = true;
        text.pop_back();
    }

    if (text.empty())
    {
        return san;
    }

    if (is_file(text.back()))
    {
        san->src_x = text.back() - 'a';
        text.pop_back();
    }
    else if (is_rank(text.back()))
    {
        san->src_y = text.back() - '1';
        text.pop_back();
    }

    if (text.empty())
    {
        return san;
    }

    // If we've got this far then the previous character should have been a rank, and we now
    // expect a file.
    if (is_file(text.back()))
    {
        san->src_x = text.back() - 'a';
        text.pop_back();
    }

    return san;
}

std::unique_ptr<Token> Lexer::consume_tag_open_or_movetext()
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

std::unique_ptr<Token> Lexer::consume_tag_open()
{
    auto const ch = m_stream.get();

    if (ch == '[')
    {
        m_state = State::expect_tag_name;
        return std::make_unique<TagPairOpen>();
    }
    else
    {
        m_state = State::error;
        return std::make_unique<SyntaxError>();
    }
}

std::unique_ptr<Token> Lexer::consume_movetext()
{
    auto ch = m_stream.peek();
    auto str = std::string{};

    if (ch == '{')
    {
        if (auto error = consume_comment(); error)
        {
            return error;
        }
        consume_whitespace(m_stream);
    }

    ch = m_stream.get();

    while (!isspace(ch) && ch != '.' && ch != -1)
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
            return std::make_unique<SyntaxError>();
        }

        m_state = State::expect_colour_indicator;
        return std::make_unique<MoveNumber>(number);
    }
}

std::unique_ptr<Token> Lexer::consume_tag_name()
{
    auto ch = m_stream.get();
    auto name = std::string{};

    while (isalnum(ch) || ch == '_')
    {
        name += static_cast<char>(ch);
        ch = m_stream.get();
    }

    m_state = State::expect_tag_value;
    return std::make_unique<TagPairName>(std::move(name));
}

std::unique_ptr<Token> Lexer::consume_comment()
{
    auto ch = m_stream.get();

    while (ch != '}')
    {
        if (ch == -1)
        {
            m_state = State::error;
            return std::make_unique<SyntaxError>();
        }
        ch = m_stream.get();
    }

    return nullptr;
}

std::unique_ptr<Token> Lexer::consume_colour_indicator()
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
        return std::make_unique<SyntaxError>();
    }

    m_state = State::expect_movetext;
    return std::make_unique<ColourIndicator>(colour);
}

std::unique_ptr<Token> Lexer::consume_tag_value()
{
    auto ch = m_stream.get();
    auto value = std::string{};

    if (ch != '"')
    {
        m_state = State::error;
        return std::make_unique<SyntaxError>();

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
                return std::make_unique<SyntaxError>();
            }
        }

        value += static_cast<char>(ch);
        ch = m_stream.get();
    }

    if (ch == ']')
    {
        m_state = State::error;
        return std::make_unique<SyntaxError>();
    }

    m_state = State::expect_tag_close;
    return std::make_unique<TagPairValue>(value);
}

std::unique_ptr<Token> Lexer::consume_tag_close()
{
    auto const ch = m_stream.get();

    if (ch == ']')
    {
        m_state = State::expect_tag_open_or_movetext;
        return std::make_unique<TagPairClose>();
    }

    m_state = State::error;
    return std::make_unique<SyntaxError>();
}