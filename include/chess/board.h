#pragma once
#include <array>

#include <chess/piece.h>
#include <chess/loc.h>

namespace chess
{
    struct Board
    {
        static Board standard();
        static Board blank();

        Square & operator[](Loc loc);
        Square const& operator[](Loc loc) const;

    private:
        Board() = default;
        std::array<Square, Loc::board_size> squares = {};
    };
}