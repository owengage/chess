#pragma once
#include <array>
#include <vector>
#include <utility>

#include <chess/Square.h>
#include <chess/Loc.h>

namespace chess
{
    struct Move;

    struct Board
    {
        static Board standard();
        static Board blank();
        static Board with_pieces(std::vector<std::pair<Loc, Square>> const&);

        Square & operator[](Loc loc);
        Square const& operator[](Loc loc) const;

        void force_move(Move const& move);

        Colour turn = Colour::white;
        std::optional<Loc> last_turn_pawn_double_jump_dest = std::nullopt;
    private:
        Board() = default;
        std::array<Square, Loc::board_size> squares = {};
    };
}