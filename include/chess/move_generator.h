#pragma once

#include <chess/loc.h>
#include <chess/move.h>

namespace chess
{
    struct Game;

    using MoveList = std::vector<Move>;

    struct MoveGenerator {
        MoveGenerator(Game const &game, Loc src);

        MoveList operator()(Empty p);

        template<typename T>
        MoveList operator()(T p) {
            MoveList ml;
            if (!is_pieces_turn(p)) {
                return ml;
            }

            ml = get_for(p);
            remove_checked(ml);
            return ml;
        }

    private:

        MoveList get_for(Pawn p);
        MoveList get_for(Rook p);
        MoveList get_for(Knight p);
        MoveList get_for(Bishop p);
        MoveList get_for(Queen p);
        MoveList get_for(King p);

        /**
         * Is the turn of the colour of the given piece
         */
        bool is_pieces_turn(PieceCommon const &p);

        /**
         * Location is an empty square/
         */
        bool is_empty(Loc loc);

        /**
         * Location would be a capture if moved there.
         */
        bool is_capturable(Loc loc);
        /**
         * The piece at the given location has moved before.
         */
        bool has_moved(Loc loc);

        /**
         * True if either player is in check on given board.
         */
        bool is_in_check(Board const&);

        void add(MoveList &ml, Loc dest);

        /**
         * Add a standard move or capture move, return if should stop.
         */
        bool add_standard(MoveList &ml, Loc dest);

        /**
         * Set ability to move to given offset-location. Allows move to empty square and to capture.
         */
        void add_delta(MoveList &ml, int dx, int dy);

        /**
         * Set ability to move to given offset-location only if it would be a capture.
         */
        void add_delta_capture(MoveList &ml, int dx, int dy);

        /**
         * Add ability to move to given offset-location only if it is empty
         */
        void add_delta_empty(MoveList &ml, int dx, int dy);

        /**
         * Add moves for the given direction from the source square.
         */
        void add_direction(MoveList &ml, int dx, int dy);

        void remove_checked(MoveList &ml);

        Game const& game;
        Loc src;
    };
}