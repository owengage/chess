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

    struct Pawn : public PieceCommon {};
    struct Rook : public PieceCommon {};
    struct Knight : public PieceCommon {};
    struct Bishop : public PieceCommon {};
    struct King : public PieceCommon {};
    struct Queen : public PieceCommon {};
    struct Empty {};

    inline bool operator==(Empty, Empty) { return true; }
    inline bool operator!=(Empty, Empty) { return false; }

    template<typename T, typename = std::enable_if_t<std::is_base_of_v<PieceCommon, T>, void>>
    bool operator==(T lhs, T rhs) { return lhs.colour == rhs.colour; }

    template<typename T, typename = std::enable_if_t<std::is_base_of_v<PieceCommon, T>, void>>
    bool operator!=(T lhs, T rhs) { return lhs.colour != rhs.colour; }

    using Piece = std::variant<Pawn, Rook, Knight, Bishop, King, Queen>;
    using Square = std::variant<Empty, Pawn, Rook, Knight, Bishop, King, Queen>;
}