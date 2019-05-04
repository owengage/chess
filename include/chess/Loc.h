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

    enum class Sign
    {
        positive = 1,
        none = 0,
        negative = -1
    };

    struct Loc
    {
        static constexpr int side_size = 8;
        static constexpr int board_size = side_size * side_size;

        constexpr Loc(int index) : m_index{index} {}

        constexpr Loc(int x, int y) : m_index{y*side_size+x}
        {
            if (x < 0 || y < 0 || x >= side_size || y >= side_size)
            {
                throw LocInvalid{x,y};
            }
        }

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

        static constexpr std::optional<Loc> add_delta(Loc lhs, int dx, int dy)
        {
            auto x = lhs.x() + dx;
            auto y = lhs.y() + dy;

            if (x >= 0 && x < side_size && y >= 0 &&  y < side_size)
            {
                // Recalculating index here is/was faster than using x,y constructor.
                auto delta_index = dy * side_size + dx;
                return Loc{lhs.index() + delta_index};
            }
            else
            {
                return std::nullopt;
            }
        }

        static std::vector<Loc> row(int y);
        static std::vector<Loc> const& all_squares();
        static perf::StackVector<Loc, side_size> direction(Loc origin, Sign x, Sign y);
    private:
        int m_index;

        friend bool operator==(Loc, Loc);
        friend bool operator!=(Loc, Loc);
    };

    bool operator==(Loc lhs, Loc rhs);
    bool operator!=(Loc lhs, Loc rhs);
}