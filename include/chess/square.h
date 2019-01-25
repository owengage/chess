#pragma once

#include <variant>

namespace chess
{
    enum class Colour : bool
    {
        white,
        black
    };

    struct PieceCommon
    {
        Colour colour;
    };

    struct Empty {};
    struct Pawn : public PieceCommon {};
    struct Rook : public PieceCommon {};
    struct Knight : public PieceCommon {};
    struct Bishop : public PieceCommon {};
    struct King : public PieceCommon {};
    struct Queen : public PieceCommon {};

    inline bool operator==(Empty, Empty) { return true; }
    inline bool operator!=(Empty, Empty) { return false; }
    inline bool operator==(PieceCommon const& lhs, PieceCommon const& rhs) { return lhs.colour == rhs.colour;}
    inline bool operator!=(PieceCommon const& lhs, PieceCommon const& rhs) { return lhs.colour != rhs.colour;}

    using Square = std::variant<Empty, Pawn, Rook, Knight, Bishop, King, Queen>;

    Colour get_colour(Square sq);
}