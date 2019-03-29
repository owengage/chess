#pragma once

#include <chess/pgn/parser.h>
#include <chess/pgn/tokens.h>
#include <chess/pgn/lexer.h>

#include <iosfwd>
#include <optional>

namespace chess::pgn
{
    /**
     * Parses a game in PGN for legality.
     */
    struct MoveParser : Parser
    {
        explicit MoveParser(std::istream &);

        std::optional<SanMove> next();

        void visit(TagPairOpen const &open) override;
        void visit(TagPairClose const &close) override;
        void visit(TagPairName const &name) override;
        void visit(TagPairValue const &value) override;
        void visit(MoveNumber const &number) override;
        void visit(ColourIndicator const &indicator) override;
        void visit(SanMove const &move) override;
        void visit(AlternativeOpen const &open) override;
        void visit(AlternativeClose const &close) override;
        void visit(SyntaxError const &error) override;
        void visit(TerminationMarker const &marker) override;

    private:
        Lexer m_lexer;
        std::optional<SanMove> m_move;
    };
}