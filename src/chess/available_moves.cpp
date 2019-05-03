#include <chess/available_moves.h>
#include <chess/Board.h>

#include <algorithm>
#include <vector>

namespace chess {

    namespace
    {
        bool is_pawn_double_jump(Move const& move)
        {
            auto const& sq = move.result[move.dest];
            auto const dy = move.src.y() - move.dest.y();

            return (sq.type() == SquareType::pawn) && std::abs(dy) == 2;
        }

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
            explicit PotentialMoves(Board const& board);

            std::vector<Move> retrieve();

        private:
            void generate_for(Square, Loc);
            void generate_for_pawn(Square, Loc);
            void generate_for_rook(Loc);
            void generate_for_knight(Loc);
            void generate_for_bishop(Loc);
            void generate_for_queen(Loc);
            void generate_for_king(Square, Loc);

            void generate_en_passant(Square, Loc);
            void generate_promotions(Square, Loc);

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

            void add_promotions(Loc src, Loc dest);

            void remove_checked(Colour);
            void flip_turn_for_list();

            Board const& board;
            std::vector<Move> list;
        };


        PotentialMoves::PotentialMoves(Board const& board)
                : board{board}, list{}
        {
            auto current_colour = board.turn;

            for (Loc loc : Loc::all_squares())
            {
                auto sq = board[loc];
                if (sq.colour() == current_colour)
                {
                    generate_for(sq, loc);
                }
            }

            // TODO: Better moved to pawn move generation.
            for (auto & move : list)
            {
                if (is_pawn_double_jump(move))
                {
                    move.result.last_turn_pawn_double_jump_dest = move.dest;
                }
                else
                {
                    move.result.last_turn_pawn_double_jump_dest = std::nullopt;
                }
            }

            flip_turn_for_list();
            remove_checked(current_colour);
        }

        void PotentialMoves::flip_turn_for_list()
        {
            for (auto & move : list)
            {
                move.result.turn = flip_colour(move.result.turn);
            }
        }

        std::vector<Move> PotentialMoves::retrieve()
        {
            std::vector<Move> ret;
            std::swap(ret, list);
            return ret;
        }

        void PotentialMoves::generate_for(Square sq, Loc src)
        {
            switch (sq.type())
            {
                case SquareType::pawn:
                    generate_for_pawn(sq, src);
                    break;
                case SquareType::bishop:
                    generate_for_bishop(src);
                    break;
                case SquareType::rook:
                    generate_for_rook(src);
                    break;
                case SquareType::knight:
                    generate_for_knight(src);
                    break;
                case SquareType::queen:
                    generate_for_queen(src);
                    break;
                case SquareType::king:
                    generate_for_king(sq, src);
                    break;
                case SquareType::empty:
                    break;
            }
        }

        bool PotentialMoves::is_empty(Loc loc) {
            return board[loc] == Empty();
        }

        bool PotentialMoves::is_capturable(Loc loc, Loc src) {
            auto src_colour = board[src].colour();
            return !is_empty(loc) && src_colour != board[loc].colour();
        }

        bool PotentialMoves::has_moved(Loc loc) {
            auto p = board[loc];
            return p.type() != SquareType::empty && p.has_moved();
        }

        void PotentialMoves::add(Loc dest, Loc src) {
            Board b = board;
            b[dest] = b[src];
            b[src] = Empty();
            b[dest].set_moved();
            list.push_back({src, dest, b});
        }

        void PotentialMoves::add_castling(Loc king_dest, Loc king_src, Loc rook_dest, Loc rook_src) {
            Board b = board;
            b[king_dest] = b[king_src];
            b[king_src] = Empty();
            b[rook_dest] = b[rook_src];
            b[rook_src] = Empty();

            b[rook_dest].set_moved();
            b[king_dest].set_moved();

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
            auto king_loc = find_loc_of(board, King(current_colour));

            auto new_end = std::remove_if(begin(list), end(list), [this, &king_loc](Move m) {
                return m.dest == king_loc;
            });

            list.erase(new_end, end(list));
        }

        void PotentialMoves::generate_for_pawn(Square p, Loc src) {
            int direction = p.colour() == Colour::white ? 1 : -1;

            // Only add square ahead if not at the end of the board. Promotion move dealt with specially.
            if (auto dest = Loc::add_delta(src, 0, direction); dest && (dest->y() != Loc::side_size - 1 && dest->y() != 0))
            {
                add_delta_empty(0, direction, src);
                add_delta_capture(1, direction, src);
                add_delta_capture(-1, direction, src);
            }

            // Can move two if hasn't moved before and first space free.
            auto jump_one = Loc::add_delta(src, 0, direction);
            if (jump_one && is_empty(*jump_one) && !has_moved(src)) {
                add_delta_empty(0, 2 * direction, src);
            }

            generate_en_passant(p, src);
            generate_promotions(p, src);
        }

        void PotentialMoves::add_promotions(Loc src, Loc dest)
        {
            auto new_board = board;
            new_board[dest] = new_board[src];
            new_board[src] = Empty();

            new_board[dest].set_type(SquareType::rook);
            list.push_back({src, dest, new_board, MoveType::normal, true});

            new_board[dest].set_type(SquareType::bishop);
            list.push_back({src, dest, new_board, MoveType::normal, true});

            new_board[dest].set_type(SquareType::knight);
            list.push_back({src, dest, new_board, MoveType::normal, true});

            new_board[dest].set_type(SquareType::queen);
            list.push_back({src, dest, new_board, MoveType::normal, true});
        }

        void PotentialMoves::generate_promotions(Square p, Loc src)
        {
            int direction = p.colour() == Colour::white ? 1 : -1;
            auto non_capture_dest = Loc::add_delta(src, 0, direction);
            auto left_capture_dest = Loc::add_delta(src, -1, direction);
            auto right_capture_dest = Loc::add_delta(src, 1, direction);

            // Must be a promotion if we're going to land on either end of the board.
            if (non_capture_dest && (non_capture_dest->y() == Loc::side_size - 1 || non_capture_dest->y() == 0))
            {
                if (is_empty(*non_capture_dest))
                {
                    add_promotions(src, *non_capture_dest);
                }

                if (left_capture_dest && is_capturable(*left_capture_dest, src))
                {
                    add_promotions(src, *left_capture_dest);
                }

                if (right_capture_dest && is_capturable(*right_capture_dest, src))
                {
                    add_promotions(src, *right_capture_dest);
                }
            }
        }

        void PotentialMoves::generate_en_passant(Square p, Loc src)
        {
            // If in the last move a pawn jumped 2 spaces, and we're in the position where if it moved just one we could
            // take it, we can move there and take the pawn that jumped 2 squares.

            int direction = p.colour() == Colour::white ? 1 : -1;
            auto const& current_board = board;

            if (auto const& last_move_dest = board.last_turn_pawn_double_jump_dest; last_move_dest)
            {
                bool last_move_was_to_side_of_current =
                        src.y() == last_move_dest->y() && std::abs(src.x() - last_move_dest->x()) == 1;

                if (last_move_was_to_side_of_current) {
                    auto dest = Loc::add_delta(*last_move_dest, 0, direction);
                    if (dest && is_empty(*dest)) {
                        Board b = board;
                        b[*dest] = b[src];
                        b[src] = Empty();
                        b[*last_move_dest] = Empty(); // capture en passant

                        b[*dest].set_moved();

                        list.push_back({src, *dest, b});
                    }
                }
            }
        }

        void PotentialMoves::generate_for_rook(Loc src) {
            add_direction(1, 0, src);
            add_direction(0, 1, src);
            add_direction(0, -1, src);
            add_direction(-1, 0, src);
        }

        void PotentialMoves::generate_for_knight(Loc src) {
            add_delta(1, 2, src);
            add_delta(1, -2, src);
            add_delta(-1, 2, src);
            add_delta(-1, -2, src);
            add_delta(2, 1, src);
            add_delta(2, -1, src);
            add_delta(-2, 1, src);
            add_delta(-2, -1, src);
        }

        void PotentialMoves::generate_for_bishop(Loc src) {
            add_direction(1, 1, src);
            add_direction(-1, 1, src);
            add_direction(1, -1, src);
            add_direction(-1, -1, src);
        }

        void PotentialMoves::generate_for_queen(Loc src) {
            add_direction(0, 1, src);
            add_direction(1, 0, src);
            add_direction(-1, 0, src);
            add_direction(0, -1, src);
            add_direction(1, 1, src);
            add_direction(-1, 1, src);
            add_direction(1, -1, src);
            add_direction(-1, -1, src);
        }

        void PotentialMoves::generate_for_king(Square k, Loc src) {
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
                auto colour = k.colour();
                auto left = Loc{0, src.y()};
                auto right = Loc{Loc::side_size - 1, src.y()};
                auto rook = Rook(colour);

                if (board[left] == rook && !has_moved(left))
                {
                    auto king_src = src;
                    auto king_dest = *Loc::add_delta(left, 2, 0);
                    auto rook_dest = *Loc::add_delta(left, 3, 0);

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

        std::vector<Move> potential_moves(Board const& board)
        {
            auto pm = PotentialMoves{board};
            return pm.retrieve();
        }

        bool is_castling(Move const& move)
        {
            return (move.result[move.dest].type() == SquareType::king) && std::abs(move.src.x() - move.dest.x()) > 1;
        }

        bool causes_mover_to_be_in_check(Board board, Move const& current_move)
        {
            // Must do before assuming the move.
            auto current_colour = board.turn;
            board = current_move.result;

            auto king_loc = find_loc_of(board, King(current_colour));
            auto potentials = potential_moves(board);

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

        MoveType determine_if_causes_check(Board board, Move const& current_move)
        {
            // Must do before assuming the move.
            auto opposite_colour = (board.turn == Colour::white) ? Colour::black : Colour::white;
            board = current_move.result; // make the move
            board.turn = flip_colour(board.turn);

            auto king_loc = find_loc_of(board, King(opposite_colour));
            auto potentials = potential_moves(board);

            auto it = std::find_if(begin(potentials), end(potentials), [&king_loc](Move m)
            {
                return m.dest == king_loc;
            });

            bool in_check = end(potentials) != it;

            return in_check ? MoveType::check : MoveType::normal;
        }


        MoveType determine_if_causes_checkmate(Board board, Move const& current_move)
        {
            // We know that the move given causes check for the other player
            // We want to determine if it is checkmate.
            // ie, if I make the move, the other player cannot play another move that gets them out of check.

            board = current_move.result; // make the move that causes check.
            auto potentials = potential_moves(board); // get moves for other player

            // Filter out moves that put the enemy in check.
            auto it = std::remove_if(begin(potentials), end(potentials), [&board](Move const& move)
            {
                return causes_mover_to_be_in_check(board, move);
            });
            potentials.erase(it, end(potentials));

            if (potentials.empty())
            {
                return MoveType::checkmate;
            }
            else
            {
                return MoveType::check;
            }
        }
    }

    std::vector<Move> available_moves(Board const& board)
    {
        auto potentials = potential_moves(board);

        auto it = std::remove_if(begin(potentials), end(potentials), [&board](Move const& move)
        {
            return causes_mover_to_be_in_check(board, move);
        });
        potentials.erase(it, end(potentials));

        // For each potential move, skip next player, see if current player could take king on next go
        // ie the move causes check.
        std::for_each(begin(potentials), end(potentials), [&board](Move & move) {
            move.type = determine_if_causes_check(board, move);
        });

        // Want to check for checkmate for each move that causes check.
        std::for_each(begin(potentials), end(potentials), [&board](Move & move)
        {
            if (move.type == MoveType::check)
            {
                move.type = determine_if_causes_checkmate(board, move);
            }
        });

        return potentials;
    }

}