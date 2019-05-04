#include <chess/Loc.h>
#include <perf/StackVector.h>

using chess::LocInvalid;
using chess::Loc;

namespace
{
    std::vector<Loc> create_all_locs() {
        std::vector<Loc> all;
        all.reserve(static_cast<std::size_t>(Loc::board_size));

        for (int y = 0; y < Loc::side_size; ++y) {
            for (int x = 0; x < Loc::side_size; ++x) {
                all.emplace_back(x, y);
            }
        }
        return all;
    }
}

LocInvalid::LocInvalid(int x, int y) : std::runtime_error{"Invalid Loc m_x=" + std::to_string(x) + ", y=" + std::to_string(y)}
{}

std::vector<Loc> Loc::row(int y)
{
    std::vector<Loc> locs;
    locs.reserve(side_size);

    for (int i = 0; i < side_size; ++i)
    {
        locs.emplace_back(i, y);
    }

    return locs;
}

std::vector<Loc> const& Loc::all_squares()
{
    static std::vector<Loc> all = create_all_locs();
    return all;
}

perf::StackVector<Loc, Loc::side_size> Loc::direction(Loc origin, int dx, int dy)
{
    auto locs = perf::StackVector<Loc, 8>{};

    std::optional<Loc> current = origin;
    while (current && (current = Loc::add_delta(*current, dx, dy)))
    {
        locs.push_back(*current);
    }

    return locs;
}

bool chess::operator==(Loc lhs, Loc rhs)
{
    return lhs.m_index == rhs.m_index;
    //return lhs.m_x == rhs.m_x && lhs.m_y == rhs.m_y;
}

bool chess::operator!=(Loc lhs, Loc rhs)
{
    return !(lhs == rhs);
}