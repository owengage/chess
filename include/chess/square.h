#pragma once

#include <variant>
#include <cstddef>

namespace chess
{
    enum class Colour : bool
    {
        black,
        white,
    };

    struct PieceCommon
    {
        constexpr PieceCommon(Colour c, bool moved = false) :
        m_data{((c == Colour::white) ? colour_mask : std::byte{}) | (moved ? move_mask : std::byte{})}
        {}

        constexpr bool has_moved() const { return (m_data & move_mask) == move_mask; }
        constexpr void set_moved() { m_data |= move_mask; }

        constexpr Colour colour() const { return (m_data & colour_mask) == colour_mask ? Colour::white : Colour::black; }
    private:
        static std::byte constexpr colour_mask{1U << 1};
        static std::byte constexpr move_mask{1U << 2};

        std::byte m_data;
    };

    struct Empty {};
    struct Pawn : public PieceCommon { using PieceCommon::PieceCommon; };
    struct Rook : public PieceCommon { using PieceCommon::PieceCommon; };
    struct Knight : public PieceCommon { using PieceCommon::PieceCommon; };
    struct Bishop : public PieceCommon { using PieceCommon::PieceCommon; };
    struct King : public PieceCommon { using PieceCommon::PieceCommon; };
    struct Queen : public PieceCommon { using PieceCommon::PieceCommon; };

    inline bool operator==(Empty, Empty) { return true; }
    inline bool operator!=(Empty, Empty) { return false; }
    inline bool operator==(PieceCommon const& lhs, PieceCommon const& rhs) { return lhs.colour() == rhs.colour();}
    inline bool operator!=(PieceCommon const& lhs, PieceCommon const& rhs) { return lhs.colour() != rhs.colour();}

    using Square = std::variant<Empty, Pawn, Rook, Knight, Bishop, King, Queen>;

    Colour get_colour(Square sq);
    Colour flip_colour(Colour);
    bool is_colour(Square sq, Colour c);

    bool has_moved(Square sq);
    void set_moved(Square & sq);
}