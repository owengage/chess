#include <chess/Loc.h>
#include <perf/StackVector.h>

#include <unordered_map>

using chess::LocInvalid;
using chess::Loc;
using chess::Sign;

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

    perf::StackVector<Loc, Loc::side_size> generate_direction(Loc origin, Sign x, Sign y)
    {
        auto locs = perf::StackVector<Loc, 8>{};

        auto sign_to_hops = [](Sign sign, int start)
        {
            switch (sign)
            {
                case Sign::positive:
                    return Loc::side_size - 1 - start;
                case Sign::none:
                    return Loc::side_size - 1;
                case Sign::negative:
                    return start;
            }
        };

        auto max_hops_x = sign_to_hops(x, origin.x());
        auto max_hops_y = sign_to_hops(y, origin.y());
        auto max_hops = std::min(max_hops_x, max_hops_y);
        auto hop = static_cast<int>(y)*Loc::side_size + static_cast<int>(x);

        auto current = origin.index();
        for (int i = 0; i < max_hops; ++i)
        {
            current += hop;
            locs.push_back(Loc{current});
        }

        return locs;
    }

        std::size_t direction_lookup_index(Loc loc, Sign x, Sign y)
        {
            auto loc_part = loc.index() << 4;
            auto x_part = (static_cast<int>(x) + 1) << 2;
            auto y_part = (static_cast<int>(y) + 1);
            return loc_part | x_part | y_part;
        }

        static constexpr std::size_t direction_lookup_size = (64 << 4) + (2 << 2) + 2 + 1;

    std::array<perf::StackVector<Loc, 8>, direction_lookup_size> generate_direction_lookup_map()
    {
        std::array<perf::StackVector<Loc, 8>, direction_lookup_size> map;
        std::array<Sign, 3> signs = {Sign::positive, Sign::none, Sign::negative};

        for (auto loc : Loc::all_squares())
        {
            for (auto x : signs)
            {
                for (auto y : signs)
                {
                    auto key = direction_lookup_index(loc, x, y);
                    auto value = generate_direction(loc, x, y);
                    map[key] = value;
                }
            }
        }

        return map;
    }

    auto direction_lookup = generate_direction_lookup_map();
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

perf::StackVector<Loc, Loc::side_size> Loc::direction(Loc origin, Sign x, Sign y)
{
    return direction_lookup[direction_lookup_index(origin, x, y)];
}

bool chess::operator==(Loc lhs, Loc rhs)
{
    return lhs.m_index == rhs.m_index;
}

bool chess::operator!=(Loc lhs, Loc rhs)
{
    return !(lhs == rhs);
}