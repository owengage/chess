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
            auto current_colour = ((game.history().size() % 2) == 0) ? Colour::white : Colour::black;
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
            for (auto const &b : game.history()) {
                if (b.result[loc] != p) {
                    return true;
                }
            }

            return false;
        }

        bool PotentialMoves::is_in_check(Board const& b, Colour current_colour) {
            auto king_loc = find_loc_of(b, Square{King{current_colour}});

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
            list.push_back({src, dest, b});
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
            auto const &history = game.history();
            if (!history.empty()) {
                auto const &last_move = history.back();
                bool last_move_was_pawn = std::holds_alternative<Pawn>(last_move.result[last_move.dest]);
                bool last_move_was_to_side_of_current =
                        src.y() == last_move.dest.y() && std::abs(src.x() - last_move.dest.x()) == 1;

                if (last_move_was_pawn && last_move_was_to_side_of_current) {
                    auto dest = Loc::add_delta(last_move.dest, 0, direction);
                    if (dest && is_empty(*dest)) {
                        Board b = game.current();
                        b[*dest] = b[src];
                        b[src] = Square{Empty{}};
                        b[last_move.dest] = Square{Empty{}}; // capture en passant

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

        void PotentialMoves::generate_for(King, Loc src) {
            add_delta(0, 1, src);
            add_delta(1, 1, src);
            add_delta(1, 0, src);
            add_delta(1, -1, src);
            add_delta(0, -1, src);
            add_delta(-1, -1, src);
            add_delta(-1, 0, src);
            add_delta(-1, 1, src);
        }

        void PotentialMoves::generate_for(Empty, Loc) {
        }

        std::vector<Move> potential_moves(Game const& game)
        {
            auto pm = PotentialMoves{game};
            return pm.retrieve();
        }

        bool causes_mover_to_be_in_check(Game & game, Move const& move)
        {
            // Must do before assuming the move.
            auto current_colour = ((game.history().size() % 2) == 0) ? Colour::white : Colour::black;

            auto token = game.assume_move(move);
            auto king_loc = find_loc_of(game.current(), Square{King{current_colour}});
            auto potentials = potential_moves(game);

            auto it = std::find_if(begin(potentials), end(potentials), [&king_loc](Move m)
            {
                return m.dest == king_loc;
            });

            return it != end(potentials);
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
        return potentials;
    }

}