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
                m_index{(letternum[1] - '1')*side_size + (letternum[0] - 'A')}
        {
            if (x() >= side_size || y() >= side_size)
            {
                throw LocInvalid{x(),y()};
            }
        }

        constexpr int x() const
        {
            return m_index % side_size;
        }

        constexpr int y() const
        {
            return m_index / side_size;
        }

        constexpr int index() const
        {
            return m_index;
        }

        static std::vector<Loc> row(int y);
        static std::vector<Loc> const& all_squares();
        static std::optional<Loc> add_delta(Loc lhs, int x, int y);
        static perf::StackVector<Loc, 8> direction(Loc origin, int dx, int dy);
    private:
        int m_index;

        friend Loc operator+(Loc, Loc);
        friend bool operator==(Loc, Loc);
        friend bool operator!=(Loc, Loc);
    };

    Loc operator+(Loc lhs, Loc rhs);
    bool operator==(Loc lhs, Loc rhs);
    bool operator!=(Loc lhs, Loc rhs);
}