#pragma once

#include <chess/pgn/tokens.h>

namespace chess::pgn
{
    struct Parser
    {
        virtual ~Parser() = default;

        virtual void visit(TagPairOpen const&) = 0;
        virtual void visit(TagPairClose const&) = 0;
        virtual void visit(TagPairName const&) = 0;
        virtual void visit(TagPairValue const&) = 0;
        virtual void visit(MoveNumber const&) = 0;
        virtual void visit(ColourIndicator const&) = 0;
        virtual void visit(SanMove const&) = 0;
        virtual void visit(AlternativeOpen const&) = 0;
        virtual void visit(AlternativeClose const&) = 0;
        virtual void visit(SyntaxError const&) = 0;
        virtual void visit(TerminationMarker const&) = 0;
    };
}