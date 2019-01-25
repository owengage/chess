#include <chess/game.h>
#include <chess/move_generator.h>
#include <vector>

namespace chess {

    namespace
    {
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

    }

    MoveGenerator::MoveGenerator(Game const &game, Loc src)
    : game{game}, src{src} {}


        bool MoveGenerator::is_pieces_turn(PieceCommon const &p) {
            return ((game.history().size() % 2) == 1) != (p.colour == Colour::white);
        }


        bool MoveGenerator::is_empty(Loc loc) {
            return game.current()[loc] == Square{Empty{}};
        }


        bool MoveGenerator::is_capturable(Loc loc) {
            auto const board = game.current();
            auto src_colour = get_colour(board[src]);
            return !is_empty(loc) && src_colour != get_colour(board[loc]);
        }

        bool MoveGenerator::has_moved(Loc loc) {
            auto p = game.current()[loc];
            for (auto const &b : game.history()) {
                if (b.result[loc] != p) {
                    return true;
                }
            }

            return false;
        }

        void MoveGenerator::add(MoveList &ml, Loc dest) {
            Board b = game.current();
            b[dest] = b[src];
            b[src] = Square{Empty{}};
            ml.push_back({src, dest, b});
        }

        bool MoveGenerator::add_standard(MoveList &ml, Loc dest) {
            bool empty = is_empty(dest);
            bool capture = is_capturable(dest);
            bool occupied = capture || !empty;

            if (empty || capture) {
                add(ml, dest);
            }

            return occupied;
        }

        void MoveGenerator::add_delta(MoveList &ml, int dx, int dy) {
            auto dest = Loc::add_delta(src, dx, dy);
            if (dest && (is_empty(*dest) || is_capturable(*dest))) {
                add(ml, *dest);
            }
        }

        void MoveGenerator::add_delta_capture(MoveList &ml, int dx, int dy) {
            auto dest = Loc::add_delta(src, dx, dy);
            if (dest && is_capturable(*dest)) {
                add(ml, *dest);
            }
        }

        void MoveGenerator::add_delta_empty(MoveList &ml, int dx, int dy) {
            auto dest = Loc::add_delta(src, dx, dy);
            if (dest && is_empty(*dest)) {
                add(ml, *dest);
            }
        }

        void MoveGenerator::add_direction(MoveList &ml, int dx, int dy) {
            for (Loc loc : Loc::direction(src, dx, dy)) {
                if (add_standard(ml, loc)) {
                    break;
                }
            }
        }

        MoveList MoveGenerator::operator()(Empty p) {
            return {};
        }

        MoveList MoveGenerator::get_for(Pawn p) {
            MoveList moves;
            int direction = p.colour == Colour::white ? 1 : -1;

            add_delta_empty(moves, 0, direction);
            add_delta_capture(moves, 1, direction);
            add_delta_capture(moves, -1, direction);

            // Can move two if hasn't moved before and first space free.
            auto jump_one = Loc::add_delta(src, 0, direction);
            if (jump_one && is_empty(*jump_one) && !has_moved(src)) {
                add_delta_empty(moves, 0, 2 * direction);
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

                        moves.push_back({src, *dest, b});
                    }
                }
            }

            return moves;
        }

        MoveList MoveGenerator::get_for(Rook p) {
            MoveList ml{};

            add_direction(ml, 0, 1);
            add_direction(ml, 1, 0);
            add_direction(ml, -1, 0);
            add_direction(ml, 0, -1);

            return ml;
        }

        MoveList MoveGenerator::get_for(Knight p) {
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

        MoveList MoveGenerator::get_for(Bishop p) {
            MoveList ml;
            add_direction(ml, 1, 1);
            add_direction(ml, -1, 1);
            add_direction(ml, 1, -1);
            add_direction(ml, -1, -1);
            return ml;
        }

        MoveList MoveGenerator::get_for(Queen p) {
            MoveList ml;
            add_direction(ml, 0, 1);
            add_direction(ml, 1, 0);
            add_direction(ml, -1, 0);
            add_direction(ml, 0, -1);
            add_direction(ml, 1, 1);
            add_direction(ml, -1, 1);
            add_direction(ml, 1, -1);
            add_direction(ml, -1, -1);
            return ml;
        }

        MoveList MoveGenerator::get_for(King p) {
            MoveList ml;
            add_delta(ml, 0, 1);
            add_delta(ml, 1, 1);
            add_delta(ml, 1, 0);
            add_delta(ml, 1, -1);
            add_delta(ml, 0, -1);
            add_delta(ml, -1, -1);
            add_delta(ml, -1, 0);
            add_delta(ml, -1, 1);
            return ml;
        }
}