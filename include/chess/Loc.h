#pragma once

#include <perf/StackVector.h>

#include <stdexcept>
#include <vector>
#include <optional>

namespace chess
{
    struct LocInvalid : public std::runtime_error
    {
        LocInvalid(int x, int y);
    };

    struct Loc
    {
        static constexpr int side_size = 8;
        static constexpr int board_size = side_size * side_size;

        Loc(int x, int y);

        constexpr Loc(char const * letternum) :
                m_x{letternum[0] - 'A'},
                m_y{letternum[1] - '1'}
        {
            if (m_x >= side_size || m_y >= side_size)
            {
                throw LocInvalid{m_x,m_y};
            }
        }

        int x() const;
        int y() const;
        int index() const;

        static std::vector<Loc> row(int y);
        static std::vector<Loc> const& all_squares();
        static std::optional<Loc> add_delta(Loc lhs, int x, int y);
        static perf::StackVector<Loc, 8> direction(Loc origin, int dx, int dy);
    private:
        int m_x;
        int m_y;

        friend Loc operator+(Loc, Loc);
        friend bool operator==(Loc, Loc);
        friend bool operator!=(Loc, Loc);
    };

    Loc operator+(Loc lhs, Loc rhs);
    bool operator==(Loc lhs, Loc rhs);
    bool operator!=(Loc lhs, Loc rhs);
}