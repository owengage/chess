#include <chess/pgn/resolve_move.h>

#include <chess/Board.h>
#include <chess/Move.h>
#include <chess/available_moves.h>
#include <chess/pgn/tokens.h>

#include <algorithm>

using chess::Loc;
using chess::Board;
using chess::Square;
using chess::SquareType;
using chess::Colour;
using chess::Move;
using chess::pgn::SanMove;

namespace
{
    auto constexpr white_king_loc = Loc{"E1"};
    auto constexpr white_kingside_rook_loc = Loc{"H1"};
    auto constexpr white_queenside_rook_loc = Loc{"A1"};

    auto constexpr black_king_loc = Loc{"E8"};
    auto constexpr black_kingside_rook_loc = Loc{"H8"};
    auto constexpr black_queenside_rook_loc = Loc{"A8"};

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

    bool is_en_passant(Move const& move)
    {
        auto dx = std::abs(move.src.x() - move.dest.x());
        auto dy = std::abs(move.src.y() - move.dest.y());
        auto type = move.result[move.dest].type();

        return dx == 1 && dy == 1 && type == SquareType::pawn;
    }

    bool is_capture(Board const& board, Move const& move)
    {
        if (is_en_passant(move))
        {
            return true;
        }
        return board[move.dest].type() != SquareType::empty
                && board[move.dest].colour() != move.result[move.dest].colour();
    }

    bool caused_mate(Board board, Move const &move)
    {
        // HACK: If opponent has no king, you can't be checkmated
        if (!find_loc_of(board, chess::King(flip_colour(board.turn))))
        {
            return false;
        }

        board = move.result;
        auto next_moves = chess::available_moves(board);
        return next_moves.empty();
    }

    Loc get_castling_rook(Board const& board, SanMove const& san)
    {
        if (board.turn == Colour::white)
        {
            return san.king_side_castle ? white_kingside_rook_loc : white_queenside_rook_loc;
        }
        else
        {
            return san.queen_side_castle ? black_queenside_rook_loc : black_kingside_rook_loc;
        }
    }

    Loc castling_king_dest(Loc king_loc, SanMove const& san)
    {
        if (san.king_side_castle)
        {
            return *Loc::add_delta(king_loc, 2, 0);
        }
        else
        {
            return *Loc::add_delta(king_loc, -2, 0);
        }
    }
}

std::optional<Move> chess::pgn::resolve_move(SanMove const& san, Board const& board)
{
    auto moves = available_moves(board);

    if (san.dest_x && san.dest_y)
    {
        // Remove moves for other pieces.
        auto remove_it = std::remove_if(begin(moves), end(moves), [&san](Move m)
        {
            return san.type != m.result[m.dest].type() && !(san.type == SquareType::pawn && san.promotion);
        });
        moves.erase(remove_it, end(moves));

        auto dest = Loc{*san.dest_x, *san.dest_y};
        remove_it = std::remove_if(begin(moves), end(moves), [&dest, &san, &board](Move m)
        {
            bool correct_promotion_flag = san.promotion.has_value() == m.is_promotion;
            bool correct_type_or_promoted = san.type == m.result[m.dest].type() || (san.type == SquareType::pawn && san.promotion);
            bool same_destination = dest == m.dest;
            bool correct_src_x_if_present = !san.src_x || san.src_x == m.src.x();
            bool correct_src_y_if_present = !san.src_y || san.src_y == m.src.y();
            bool correct_capture_flag = san.capture == is_capture(board, m);

            bool valid_move = same_destination
                    && correct_promotion_flag
                    && correct_type_or_promoted
                    && correct_src_x_if_present
                    && correct_src_y_if_present
                    && correct_capture_flag;

            if (!valid_move)
            {
                return true;
            }

            auto is_mate = caused_mate(board, m);

            // Complicated logic since the SAN does not say 'in check' if it's marked as checkmate.
            bool marked_check_and_isnt = san.check && !m.caused_check;
            bool marked_checkmate_but_isnt = san.checkmate != is_mate;
            bool not_marked_check_but_caused_check = !san.check && !is_mate && m.caused_check;
            bool stalemate = is_mate && !m.caused_check;

            return marked_check_and_isnt
                    || not_marked_check_but_caused_check
                    || (marked_checkmate_but_isnt && !stalemate);
        });
        moves.erase(remove_it, end(moves));
    }
    else if (san.king_side_castle || san.queen_side_castle)
    {
        auto const king_loc = board.turn == Colour::white ? white_king_loc : black_king_loc;
        auto const rook_loc = get_castling_rook(board, san);

        if (board[king_loc] != King(board.turn) || board[rook_loc] != Rook(board.turn))
        {
            return std::nullopt;
        }

        auto remove_it = std::remove_if(begin(moves), end(moves), [&board, &king_loc, &san](Move m)
        {
            return m.result[castling_king_dest(king_loc, san)] != King(board.turn);
        });
        moves.erase(remove_it, end(moves));
    }

    if (san.promotion)
    {
        auto remove_it = std::remove_if(begin(moves), end(moves), [&san](Move m)
        {
            return m.result[m.dest].type() != san.promotion;
        });
        moves.erase(remove_it, end(moves));
    }

    if (moves.size() == 1)
    {
        return moves.front();
    }

    return std::nullopt;
}