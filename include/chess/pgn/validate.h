#pragma once

#include <chess/pgn/tokens.h>

#include <vector>

namespace chess::pgn
{
    struct ValidationResult
    {
        explicit operator bool() const;
        std::optional<SanMove> invalid_move;
    };

    ValidationResult validate(std::vector<SanMove> const& moves);
}