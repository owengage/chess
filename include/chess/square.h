#pragma once

#include <variant>
#include <cstddef>

namespace chess
{
    enum class SquareType
    {
        empty,
        pawn,
        rook,
        knight,
        bishop,
        queen,
        king,
    };

    enum class Colour : bool
    {
        black,
        white,
    };

    /**
     * A square on a chess board. This is designed to be super compact to make sure a chess board can fit into a cache
     * line on a CPU. We pack all the information into 1 byte.
     *
     * This used to be std::variant, but that forced our size to be at least 2 bytes.
     */
    struct Square
    {
        constexpr Square() : m_data{}
        {}

        constexpr Square(SquareType type, Colour c, bool moved = false) :
        m_data{
                ((c == Colour::white) ? colour_mask : std::byte{})
                | (moved ? move_mask : std::byte{})
                | static_cast<std::byte>(type)}
        {}

        constexpr bool has_moved() const { return (m_data & move_mask) == move_mask; }
        constexpr void set_moved() { m_data |= move_mask; }

        constexpr Colour colour() const { return (m_data & colour_mask) == colour_mask ? Colour::white : Colour::black; }
        constexpr SquareType type() const { return static_cast<SquareType>(m_data & type_mask); }
    private:
        static std::byte constexpr type_mask   {0b0000'1111};
        static std::byte constexpr colour_mask {0b1000'0000};
        static std::byte constexpr move_mask   {0b0100'0000};

        std::byte m_data;

        friend constexpr bool operator==(Square const&, Square const&);
        friend constexpr bool operator!=(Square const&, Square const&);
    };

    constexpr bool operator==(Square const& s1, Square const& s2)
    {
        return (s1.m_data & ~Square::move_mask) == (s2.m_data & ~Square::move_mask);
    }

    constexpr bool operator!=(Square const& s1, Square const& s2)
    {
        return !(s1 == s2);
    }

    constexpr Square Empty() { return Square{SquareType::empty, {}}; }
    constexpr Square Pawn(Colour c) { return Square{SquareType::pawn, c}; }
    constexpr Square Rook(Colour c) { return Square{SquareType::rook, c}; }
    constexpr Square Knight(Colour c) { return Square{SquareType::knight, c}; }
    constexpr Square Bishop(Colour c) { return Square{SquareType::bishop, c}; }
    constexpr Square Queen(Colour c) { return Square{SquareType::queen, c}; }
    constexpr Square King(Colour c) { return Square{SquareType::king, c}; }

    Colour get_colour(Square sq);
    Colour flip_colour(Colour);
    bool is_colour(Square sq, Colour c);

    bool has_moved(Square sq);
    void set_moved(Square & sq);
}