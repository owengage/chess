#pragma once

#include <iosfwd>

#include <chess/board.h>

namespace chess::text
{
    void print(std::ostream &, Board const&);
}