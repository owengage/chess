#include <cstdint>

#include <chess/game.h>
#include <algorithm>

using chess::Loc;
using chess::Game;
using chess::Board;
using chess::Square;
using chess::Empty;
using chess::PieceCommon;
using chess::Pawn;
using chess::Rook;
using chess::Knight;
using chess::Bishop;
using chess::Queen;
using chess::King;
using chess::Colour;
using chess::Move;

namespace
{
    using MoveList = std::vector<Move>;

    struct ColourVisitor
    {
        Colour operator()(PieceCommon const& p)
        {
            return p.colour;
        }

        Colour operator()(Empty)
        {
            throw std::runtime_error{"Tried to get colour of empty square"};
        }
    };

    Colour get_colour(Square sq)
    {
        return std::visit(ColourVisitor{}, sq);
    }

    struct PatternVisitor
    {
        PatternVisitor(Game const& game, Loc src, Loc dest)
        : game{game}, src{src}, dest{dest}
        {}

        /**
         * Is the turn of the colour of the given piece
         */
        bool is_pieces_turn(PieceCommon const& p)
        {
            return ((game.history().size() % 2) == 0) != (p.colour == Colour::white);
        }

        /**
         * Location is an empty square/
         */
        bool is_empty(Loc loc)
        {
            return game.current()[loc] == Square{Empty{}};
        }

        /**
         * Location would be a capture if moved there.
         */
        bool is_capturable(Loc loc)
        {
            auto const board = game.current();
            auto src_colour = get_colour(board[src]);
            return !is_empty(loc) && src_colour != get_colour(board[loc]);
        }

        /**
         * The piece at the given location has moved before.
         */
        bool has_moved(Loc loc)
        {
            auto p = game.current()[loc];
            for (auto const& b : game.history())
            {
                if (b.result[loc] != p)
                {
                    return true;
                }
            }

            return false;
        }

        /**
         * Set ability to move to given offset-location. Allows move to empty square and to capture.
         */
        void add(MoveList &ml, Loc origin, int dx, int dy)
        {
            auto dest = Loc::add_delta(origin, dx, dy);
            if (dest && (is_empty(*dest) || is_capturable(*dest)))
            {
                Board b = game.current();
                b[*dest] = b[origin];
                b[origin] = Square{Empty{}};

                Move m{origin, *dest, b};
                ml.push_back(m);
            }
        }

        /**
         * Set ability to move to given offset-location only if it would be a capture.
         */
        void add_capturable(MoveList &ml, Loc origin, int dx, int dy)
        {
            auto dest = Loc::add_delta(origin, dx, dy);
            if (dest && is_capturable(*dest))
            {
                Board b = game.current();
                b[*dest] = b[origin];
                b[origin] = Square{Empty{}};

                Move m{origin, *dest, b};
                ml.push_back(m);
            }
        }

        /**
         * Add ability to move to given offset-location only if it is empty
         */
        void add_if_empty(MoveList & ml, Loc origin, int dx, int dy)
        {
            auto dest = Loc::add_delta(origin, dx, dy);
            if (dest && is_empty(*dest))
            {
                Board b = game.current();
                b[*dest] = b[origin];
                b[origin] = Square{Empty{}};

                Move m{origin, *dest, b};
                ml.push_back(m);
            }
        }

        MoveList operator()(Empty p)
        {
            return {};
        }

        template<typename T>
        MoveList operator()(T p)
        {
            MoveList pat;
            if (is_pieces_turn(p))
            {
                return pat;
            }

            pat = get_for(p);
            return pat;
        }

        MoveList get_for(Pawn p)
        {
            MoveList pat;
            int direction = p.colour == Colour::white ? 1 : -1;

            add_if_empty(pat, src, 0, direction);
            add_capturable(pat, src, 1, direction);
            add_capturable(pat, src, -1, direction);
            if (!has_moved(src))
            {
                add_if_empty(pat, src, 0, 2*direction);
            }

            // en passant
            // If in the last move a pawn jumped 2 spaces, and we're in the position where if it moved just one we could
            // take it, we can move there and take the pawn that jumped 2 squares.

            // Was last move a pawn?
            auto const& history = game.history();
            if (!history.empty())
            {
                auto last_move_dest = history.back().dest;
                auto const& sq = history.back().result[last_move_dest];
                if (std::holds_alternative<Pawn>(sq))
                {
                    if (src.y() == last_move_dest.y() && std::abs(src.x() - last_move_dest.x()) == 1)
                    {
                        auto dest = Loc::add_delta(last_move_dest, 0, direction);
                        if (dest && is_empty(*dest))
                        {
                            Board b = game.current();
                            b[*dest] = b[src];
                            b[src] = Square{Empty{}};
                            b[last_move_dest] = Square{Empty{}};

                            Move m{src, *dest, b};
                            pat.push_back(m);
                        }
                    }
                }
            }
            // for each enemy pawn
            //      did it move last turn?
            //      is it exactly left or right of this pawn?
            //      then somehow signal a capture?

            return pat;
        }

        MoveList get_for(Rook p)
        {
            return {};
        }

        MoveList get_for(Knight p)
        {
            MoveList pat;
            add(pat, src, 1, 2);
            add(pat, src, 1, -2);
            add(pat, src, -1, 2);
            add(pat, src, -1, -2);
            add(pat, src, 2, 1);
            add(pat, src, 2, -1);
            add(pat, src, -2, 1);
            add(pat, src, -2, -1);
            return pat;
        }

        MoveList get_for(Bishop p)
        {
            return {};
        }

        MoveList get_for(Queen p)
        {
            return {};
        }

        MoveList get_for(King p)
        {
            return {};
        }

        Game const& game;
        Loc src;
        Loc dest;
    };

    MoveList get_moves(Game const& game, Loc src, Loc dest)
    {
        auto sq = game.current()[src];
        auto pv = PatternVisitor{game, src, dest};
        return std::visit(pv, sq);
    }
}

Game::Game() : Game{Board::standard()}
{}

Game::Game(Board b) : m_start{std::move(b)}
{}

Board Game::current() const
{
    if (m_history.size() > 0)
    {
        return m_history.back().result;
    }
    return m_start;
}

std::vector<Move> const& Game::history() const
{
    return m_history;
}

bool Game::move(Loc src, Loc dest)
{
    auto board = current();
    auto moves = get_moves(*this, src, dest);
    auto move_it = std::find_if(begin(moves), end(moves), [dest](Move m) { return m.dest == dest; });

    if (move_it != std::end(moves))
    {
        board[dest] = board[src];
        board[src] = Empty{};
        m_history.push_back(*move_it);
        return true;
    }

    return false;
}
