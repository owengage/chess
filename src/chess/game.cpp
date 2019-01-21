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

        void add(MoveList &ml, Loc dest)
        {
            Board b = game.current();
            b[dest] = b[src];
            b[src] = Square{Empty{}};
            ml.push_back({src, dest, b});
        }

        /**
         * Add a standard move or capture move, return if should stop.
         */
        bool add_standard(MoveList & ml, Loc dest)
        {
            bool empty = is_empty(dest);
            bool capture = is_capturable(dest);
            bool occupied = capture || !empty;

            if (empty || capture)
            {
                add(ml, dest);
            }

            return occupied;
        }

        /**
         * Set ability to move to given offset-location. Allows move to empty square and to capture.
         */
        void add_delta(MoveList &ml, int dx, int dy)
        {
            auto dest = Loc::add_delta(src, dx, dy);
            if (dest && (is_empty(*dest) || is_capturable(*dest)))
            {
                add(ml, *dest);
            }
        }

        /**
         * Set ability to move to given offset-location only if it would be a capture.
         */
        void add_delta_capture(MoveList &ml, int dx, int dy)
        {
            auto dest = Loc::add_delta(src, dx, dy);
            if (dest && is_capturable(*dest))
            {
                add(ml, *dest);
            }
        }

        /**
         * Add ability to move to given offset-location only if it is empty
         */
        void add_delta_empty(MoveList &ml, int dx, int dy)
        {
            auto dest = Loc::add_delta(src, dx, dy);
            if (dest && is_empty(*dest))
            {
                add(ml, *dest);
            }
        }

        void add_upwards(MoveList & ml)
        {
            auto const x = src.x();

            for (auto y = src.y() + 1; y < Loc::side_size; ++y)
            {
                if (add_standard(ml, {x, y}))
                {
                    break;
                }
            }
        }

        void add_downwards(MoveList & ml)
        {
            auto const x = src.x();

            for (auto y = src.y() - 1 ; y >= 0; --y)
            {
                if (add_standard(ml, {x, y}))
                {
                    break;
                }
            }
        }

        void add_leftwards(MoveList & ml)
        {
            auto const y = src.y();

            for (auto x = src.x() - 1; x >= 0; --x)
            {
                if (add_standard(ml, {x, y}))
                {
                    break;
                }
            }
        }

        void add_rightwards(MoveList & ml)
        {
            auto const y = src.y();

            for (auto x = src.x() + 1; x < Loc::side_size; ++x)
            {
                if (add_standard(ml, {x, y}))
                {
                    break;
                }
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
            MoveList moves;
            int direction = p.colour == Colour::white ? 1 : -1;

            add_delta_empty(moves, 0, direction);
            add_delta_capture(moves, 1, direction);
            add_delta_capture(moves, -1, direction);

            // Can move two if hasn't moved before and first space free.
            auto jump_one = Loc::add_delta(src, 0, direction);
            if (jump_one && is_empty(*jump_one) && !has_moved(src))
            {
                add_delta_empty(moves, 0, 2 * direction);
            }

            // en passant
            // If in the last move a pawn jumped 2 spaces, and we're in the position where if it moved just one we could
            // take it, we can move there and take the pawn that jumped 2 squares.
            auto const& history = game.history();
            if (!history.empty())
            {
                auto const& last_move = history.back();
                bool last_move_was_pawn = std::holds_alternative<Pawn>(last_move.result[last_move.dest]);
                bool last_move_was_to_side_of_current = src.y() == last_move.dest.y() && std::abs(src.x() - last_move.dest.x()) == 1;

                if (last_move_was_pawn && last_move_was_to_side_of_current)
                {
                    auto dest = Loc::add_delta(last_move.dest, 0, direction);
                    if (dest && is_empty(*dest)) {
                        Board b = game.current();
                        b[*dest] = b[src];
                        b[src] = Square{Empty{}};
                        b[last_move.dest] = Square{Empty{}}; // capture en passant

                        moves.push_back({src, *dest, b});
                    }
                }
            }

            return moves;
        }

        MoveList get_for(Rook p)
        {
            MoveList ml{};

            add_upwards(ml);
            add_downwards(ml);
            add_leftwards(ml);
            add_rightwards(ml);

            return ml;
        }

        MoveList get_for(Knight p)
        {
            MoveList ml;
            add_delta(ml, 1, 2);
            add_delta(ml, 1, -2);
            add_delta(ml, -1, 2);
            add_delta(ml, -1, -2);
            add_delta(ml, 2, 1);
            add_delta(ml, 2, -1);
            add_delta(ml, -2, 1);
            add_delta(ml, -2, -1);
            return ml;
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
    if (!m_history.empty())
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
