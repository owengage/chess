#pragma once
#include <array>
#include <vector>
#include <utility>

#include <chess/square.h>
#include <chess/loc.h>

namespace chess
{
    struct Board
    {
        static Board standard();
        static Board blank();
        static Board with_pieces(std::vector<std::pair<Loc, Square>> const&);

        Square & operator[](Loc loc);
        Square const& operator[](Loc loc) const;

    private:
        Board() = default;
        std::array<Square, Loc::board_size> squares = {};
    };
}