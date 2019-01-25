#pragma once
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
        Loc(char const *);

        int x() const;
        int y() const;
        int index() const;

        static std::vector<Loc> row(int y);
        static std::vector<Loc> const& all_squares();
        static std::optional<Loc> add_delta(Loc lhs, int x, int y);
        static std::vector<Loc> direction(Loc origin, int dx, int dy);
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