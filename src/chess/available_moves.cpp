#include <chess/available_moves.h>

#include <algorithm>
#include <vector>

#include <chess/game.h>

namespace chess {

    namespace
    {
        std::optional<Loc> find_loc_of(Board const &b, Square sq)
        {
            for (auto const& loc : Loc::all_squares())
            {
                auto other = b[loc];
                if (other == sq)
                {
                    return loc;
                }
            }

            return std::nullopt;
        }

        struct PotentialMoves {
            explicit PotentialMoves(Game const& game);

            std::vector<Move> retrieve();

        private:
            void generate_for(Square, Loc);
            void generate_for(Pawn, Loc);
            void generate_for(Rook, Loc);
            void generate_for(Knight, Loc);
            void generate_for(Bishop, Loc);
            void generate_for(Queen, Loc);
            void generate_for(King, Loc);
            void generate_for(Empty, Loc);

            /**
             * Location is an empty square/
             */
            bool is_empty(Loc loc);

            /**
             * Location would be a capture if moved there.
             */
            bool is_capturable(Loc loc, Loc src);
            /**
             * The piece at the given location has moved before.
             */
            bool has_moved(Loc loc);

            /**
             * True if either player is in check on given board.
             */
            bool is_in_check(Board const&, Colour);

            void add(Loc dest, Loc src);


            void add_castling(Loc king_dest, Loc king_src, Loc rook_dest, Loc rook_src);

            /**
             * Add a standard move or capture move, return if should stop.
             */
            bool add_standard(Loc dest, Loc src);

            /**
             * Set ability to move to given offset-location. Allows move to empty square and to capture.
             */
            void add_delta(int dx, int dy, Loc src);

            /**
             * Set ability to move to given offset-location only if it would be a capture.
             */
            void add_delta_capture(int dx, int dy, Loc src);

            /**
             * Add ability to move to given offset-location only if it is empty
             */
            void add_delta_empty(int dx, int dy, Loc src);

            /**
             * Add moves for the given direction from the source square.
             */
            void add_direction(int dx, int dy, Loc src);

            void remove_checked(Colour);

            Game const& game;
            std::vector<Move> list;
        };


        PotentialMoves::PotentialMoves(Game const &game)
                : game{game}, list{}
        {
            auto current_colour = game.current_turn();
            auto board = game.current();

            for (Loc loc : Loc::all_squares())
            {
                auto sq = board[loc];
                if (is_colour(sq, current_colour))
                {
                    generate_for(sq, loc);
                }
            }

            remove_checked(current_colour);
        }

        std::vector<Move> PotentialMoves::retrieve()
        {
            std::vector<Move> ret;
            std::swap(ret, list);
            return ret;
        }

        void PotentialMoves::generate_for(Square sq, Loc src)
        {
            std::visit([this, src](auto sq)
                       {
                           this->generate_for(sq, src);
                       }, sq);
        }

        bool PotentialMoves::is_empty(Loc loc) {
            return game.current()[loc] == Square{Empty{}};
        }

        bool PotentialMoves::is_capturable(Loc loc, Loc src) {
            auto const board = game.current();
            auto src_colour = get_colour(board[src]);
            return !is_empty(loc) && src_colour != get_colour(board[loc]);
        }

        bool PotentialMoves::has_moved(Loc loc) {
            auto p = game.current()[loc];
            return !std::holds_alternative<Empty>(p) && chess::has_moved(p);
        }

        bool PotentialMoves::is_in_check(Board const& b, Colour current_colour) {
            auto king_loc = find_loc_of(b, Square{King{current_colour}});

            // FIXME: Deal with check if player doesn't have a king at all (useful for test matches)
//            if (king_loc == std::nullopt)
//            {
//                return false;
//            }

            for (auto const& move : list)
            {
                if (move.dest == king_loc)
                {
                    return true;
                }
            }

            return false;
        }

        void PotentialMoves::add(Loc dest, Loc src) {
            Board b = game.current();
            b[dest] = b[src];
            b[src] = Square{Empty{}};
            set_moved(b[dest], true);
            list.push_back({src, dest, b});
        }

        void PotentialMoves::add_castling(Loc king_dest, Loc king_src, Loc rook_dest, Loc rook_src) {
            Board b = game.current();
            b[king_dest] = b[king_src];
            b[king_src] = Square{Empty{}};
            b[rook_dest] = b[rook_src];
            b[rook_src] = Square{Empty{}};

            set_moved(b[rook_dest], true);
            set_moved(b[king_dest], true);

            list.push_back({king_src, king_dest, b});
        }

        bool PotentialMoves::add_standard(Loc dest, Loc src) {
            bool empty = is_empty(dest);
            bool capture = is_capturable(dest, src);
            bool occupied = capture || !empty;

            if (empty || capture) {
                add(dest, src);
            }

            return occupied;
        }

        void PotentialMoves::add_delta(int dx, int dy, Loc src) {
            auto dest = Loc::add_delta(src, dx, dy);
            if (dest && (is_empty(*dest) || is_capturable(*dest, src))) {
                add(*dest, src);
            }
        }

        void PotentialMoves::add_delta_capture(int dx, int dy, Loc src) {
            auto dest = Loc::add_delta(src, dx, dy);
            if (dest && is_capturable(*dest, src)) {
                add(*dest, src);
            }
        }

        void PotentialMoves::add_delta_empty(int dx, int dy, Loc src) {
            auto dest = Loc::add_delta(src, dx, dy);
            if (dest && is_empty(*dest)) {
                add(*dest, src);
            }
        }

        void PotentialMoves::add_direction(int dx, int dy, Loc src) {
            for (Loc loc : Loc::direction(src, dx, dy)) {
                if (add_standard(loc, src)) {
                    break;
                }
            }
        }

        void PotentialMoves::remove_checked(Colour current_colour) {
            auto king_loc = find_loc_of(game.current(), Square{King{current_colour}});

            auto new_end = std::remove_if(begin(list), end(list), [this, &king_loc](Move m) {
                return m.dest == king_loc;
            });

            list.erase(new_end, end(list));
        }

        void PotentialMoves::generate_for(Pawn p, Loc src) {
            int direction = p.colour == Colour::white ? 1 : -1;

            add_delta_empty(0, direction, src);
            add_delta_capture(1, direction, src);
            add_delta_capture(-1, direction, src);

            // Can move two if hasn't moved before and first space free.
            auto jump_one = Loc::add_delta(src, 0, direction);
            if (jump_one && is_empty(*jump_one) && !has_moved(src)) {
                add_delta_empty(0, 2 * direction, src);
            }

            // en passant
            // If in the last move a pawn jumped 2 spaces, and we're in the position where if it moved just one we could
            // take it, we can move there and take the pawn that jumped 2 squares.
            auto const& current_board = game.current();

            if (auto const& last_move_dest = game.last_turn_pawn_double_jump_dest(); last_move_dest)
            {
                bool last_move_was_to_side_of_current =
                        src.y() == last_move_dest->y() && std::abs(src.x() - last_move_dest->x()) == 1;

                if (last_move_was_to_side_of_current) {
                    auto dest = Loc::add_delta(*last_move_dest, 0, direction);
                    if (dest && is_empty(*dest)) {
                        Board b = game.current();
                        b[*dest] = b[src];
                        b[src] = Square{Empty{}};
                        b[*last_move_dest] = Square{Empty{}}; // capture en passant

                        set_moved(b[*dest], true);

                        list.push_back({src, *dest, b});
                    }
                }
            }
        }

        void PotentialMoves::generate_for(Rook, Loc src) {
            add_direction(1, 0, src);
            add_direction(0, 1, src);
            add_direction(0, -1, src);
            add_direction(-1, 0, src);
        }

        void PotentialMoves::generate_for(Knight, Loc src) {
            add_delta(1, 2, src);
            add_delta(1, -2, src);
            add_delta(-1, 2, src);
            add_delta(-1, -2, src);
            add_delta(2, 1, src);
            add_delta(2, -1, src);
            add_delta(-2, 1, src);
            add_delta(-2, -1, src);
        }

        void PotentialMoves::generate_for(Bishop, Loc src) {
            add_direction(1, 1, src);
            add_direction(-1, 1, src);
            add_direction(1, -1, src);
            add_direction(-1, -1, src);
        }

        void PotentialMoves::generate_for(Queen, Loc src) {
            add_direction(0, 1, src);
            add_direction(1, 0, src);
            add_direction(-1, 0, src);
            add_direction(0, -1, src);
            add_direction(1, 1, src);
            add_direction(-1, 1, src);
            add_direction(1, -1, src);
            add_direction(-1, -1, src);
        }

        void PotentialMoves::generate_for(King k, Loc src) {
            add_delta(0, 1, src);
            add_delta(1, 1, src);
            add_delta(1, 0, src);
            add_delta(1, -1, src);
            add_delta(0, -1, src);
            add_delta(-1, -1, src);
            add_delta(-1, 0, src);
            add_delta(-1, 1, src);

            auto empty_from_to = [this](Loc from_exclusive, Loc to_inclusive)
            {
                auto check_direction = from_exclusive.x() > to_inclusive.x() ? -1 : 1;
                auto y = from_exclusive.y();
                auto [x_min, x_max] = std::minmax({from_exclusive.x() + check_direction, to_inclusive.x()});

                for (auto x = x_min; x <= x_max; ++x)
                {
                    if (!is_empty({x, y}))
                    {
                        return false;
                    }
                }
                return true;
            };

            if (!has_moved(src))
            {
                auto board = game.current();
                auto colour = get_colour(k);
                auto left = Loc{0, src.y()};
                auto right = Loc{Loc::side_size - 1, src.y()};
                auto rook = Square{Rook{colour}};

                if (board[left] == rook && !has_moved(left))
                {
                    auto king_src = src;
                    auto king_dest = *Loc::add_delta(left, 1, 0);
                    auto rook_dest = *Loc::add_delta(left, 2, 0);

                    if (empty_from_to(king_src, king_dest))
                    {
                        add_castling(king_dest, king_src, rook_dest, left);
                    }
                }

                if (board[right] == rook && !has_moved(right))
                {
                    auto king_src = src;
                    auto king_dest = *Loc::add_delta(right, -1, 0);
                    auto rook_dest = *Loc::add_delta(right, -2, 0);

                    if (empty_from_to(king_src, king_dest))
                    {
                        add_castling(king_dest, king_src, rook_dest, right);
                    }
                }
            }
        }

        void PotentialMoves::generate_for(Empty, Loc) {
        }

        std::vector<Move> potential_moves(Game const& game)
        {
            auto pm = PotentialMoves{game};
            return pm.retrieve();
        }

        bool is_castling(Move const& move)
        {
            return std::holds_alternative<King>(move.result[move.dest]) && std::abs(move.src.x() - move.dest.x()) > 1;
        }

        bool causes_mover_to_be_in_check(Game game, Move const& current_move)
        {
            // Must do before assuming the move.
            auto current_colour = game.current_turn();
            game.force_move(current_move);

            auto king_loc = find_loc_of(game.current(), Square{King{current_colour}});
            auto potentials = potential_moves(game);

            bool in_check = end(potentials) != std::find_if(begin(potentials), end(potentials), [&king_loc](Move m)
            {
                return m.dest == king_loc;
            });

            if (!in_check && is_castling(current_move))
            {
                // Determine squares that need to be checked.
                auto vulnerable_squares = std::vector<Loc>{};
                auto y = current_move.src.y();
                auto x1 = current_move.src.x();
                auto x2 = current_move.dest.x();
                auto [x_begin, x_end] = std::minmax({x1, x2});

                for (auto x = x_begin; x <= x_end; ++x)
                {
                    vulnerable_squares.emplace_back(x,y);
                }

                in_check |= end(potentials) != std::find_if(begin(potentials), end(potentials), [&current_move, &vulnerable_squares](Move enemy_move)
                {
                    return std::find(begin(vulnerable_squares), end(vulnerable_squares), enemy_move.dest) != end(vulnerable_squares);
                });
            }

            return in_check;
        }

        bool caused_check(Game game, Move & current_move)
        {
            // Must do before assuming the move.
            auto opposite_colour = (game.current_turn() == Colour::white) ? Colour::black : Colour::white;
            game.force_move(current_move); // make the move
            game.force_move({"A1", "A1", game.current()}); // skip next move.

            auto king_loc = find_loc_of(game.current(), Square{King{opposite_colour}});
            auto potentials = potential_moves(game);

            bool in_check = end(potentials) != std::find_if(begin(potentials), end(potentials), [&king_loc](Move m)
            {
                return m.dest == king_loc;
            });

            return in_check;
        }
    }

    std::vector<Move> available_moves(Game const &game)
    {
        auto potentials = potential_moves(game);
        auto game_copy = game;

        auto it = std::remove_if(begin(potentials), end(potentials), [&game_copy](Move const& move) {
            return causes_mover_to_be_in_check(game_copy, move);
        });
        potentials.erase(it, end(potentials));

        // For each potential move, skip next player, see if current player could take king on next go
        // ie the move causes check.
        std::for_each(begin(potentials), end(potentials), [&game_copy](Move & move) {
            move.caused_check = caused_check(game_copy, move);
        });

        return potentials;
    }

}