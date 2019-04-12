#pragma once

#include <iosfwd>

#include <chess/Board.h>

namespace chess::text
{
    void print(std::ostream &, Board const&);
}