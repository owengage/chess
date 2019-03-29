#include <chess/pgn/move_parser.h>
#include <chess/pgn/tokens.h>

#include <istream>

using chess::pgn::SanMove;

using chess::pgn::MoveParser;

MoveParser::MoveParser(std::istream & stream) : m_lexer{stream, *this}
{
}

std::optional<SanMove> MoveParser::next()
{
    while (m_lexer.next())
    {
        if (m_move)
        {
            auto ret = m_move;
            m_move = std::nullopt;
            return ret;
        }
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
    m_move = move;
}

void MoveParser::visit(const AlternativeOpen &open)
{

}

void MoveParser::visit(const AlternativeClose &close)
{

}

void MoveParser::visit(const SyntaxError &error)
{

}

void MoveParser::visit(const TerminationMarker &marker)
{

}
