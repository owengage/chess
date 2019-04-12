#include <chess/pgn/MoveParser.h>
#include <chess/pgn/tokens.h>

#include <istream>

using chess::pgn::SanMove;

using chess::pgn::MoveParser;

MoveParser::MoveParser(std::istream & stream) : m_lexer{stream, *this}, m_alternative_depth{0}
{
}

std::optional<std::vector<SanMove>> MoveParser::next_game()
{
    if (!m_lexer.next())
    {
        return std::nullopt;
    }

    auto game = std::vector<SanMove>{};

    do
    {
        if (m_alternative_depth != 0)
        {
            continue;
        }

        if (m_move)
        {
            game.push_back(*m_move);
            m_move = std::nullopt;
        }

        if (m_term)
        {
            m_term = std::nullopt;
            return game;
        }


    }
    while (m_lexer.next());

    if (!m_term)
    {
        throw IncompleteGameError{"Incomplete game"};
    }

    return std::nullopt;
}

void MoveParser::visit(const TagPairOpen &open)
{

}

void MoveParser::visit(const TagPairClose &close)
{

}

void MoveParser::visit(const TagPairName &name)
{

}

void MoveParser::visit(const TagPairValue &value)
{

}

void MoveParser::visit(const MoveNumber &number)
{

}

void MoveParser::visit(const ColourIndicator &indicator)
{

}

void MoveParser::visit(const SanMove &move)
{
    if (m_alternative_depth == 0)
    {
        m_move = move;
    }
}

void MoveParser::visit(const AlternativeOpen &open)
{
    m_alternative_depth++;
}

void MoveParser::visit(const AlternativeClose &close)
{
    m_alternative_depth--;
}

void MoveParser::visit(const SyntaxError &error)
{

}

void MoveParser::visit(const TerminationMarker &marker)
{
    if (m_alternative_depth == 0)
    {
        m_term = marker;
    }
}
