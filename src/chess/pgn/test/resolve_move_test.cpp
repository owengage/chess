#include <chess/board.h>
#include <chess/game.h>
#include <chess/basic_driver.h>

#include <chess/pgn/resolve_move.h>
#include <chess/pgn/tokens.h>

#include <gtest/gtest.h>

namespace chess::pgn
{
    namespace
    {
        SanMove sanmove_with(Loc loc, SquareType type)
        {
            auto san = SanMove{};
            san.dest_x = loc.x();
            san.dest_y = loc.y();
            san.type = type;
            return san;
        }

        int file(char c)
        {
            return c - 'A';
        }

        int rank(int r)
        {
            return r - 1;
        }
    }

    TEST(resolve_move_test, empty_sanmove_does_not_resolve)
    {
        auto move = SanMove{};
        auto board = Board::standard();

        EXPECT_FALSE(resolve_move(move, board));
    }

    TEST(resolve_move_test, basic_pawn_move_resolves)
    {
        auto sanmove = SanMove{1, 2};
        sanmove.type = SquareType::pawn;

        auto board = Board::standard();
        auto move = resolve_move(sanmove, board);

        ASSERT_TRUE(move);
        EXPECT_EQ("B3", move->dest);
    }

    TEST(resolve_move_test, pawn_move_conflicting_with_knight_move_resolves)
    {
        auto sanmove = sanmove_with("A3", SquareType::pawn);
        auto board = Board::standard();
        auto move = resolve_move(sanmove, board);

        ASSERT_TRUE(move);
        EXPECT_EQ("A3", move->dest);
        EXPECT_EQ(Pawn(Colour::white), move->result[move->dest]);
        EXPECT_EQ(Empty(), move->result[move->src]);
    }

    TEST(resolve_move_test, knight_move_conflicting_with_pawn_move_resolves)
    {
        auto sanmove = sanmove_with("A3", SquareType::knight);
        auto board = Board::standard();
        auto move = resolve_move(sanmove, board);

        ASSERT_TRUE(move);
        EXPECT_EQ("A3", move->dest);
        EXPECT_EQ(Knight(Colour::white), move->result[move->dest]);
        EXPECT_EQ(Empty(), move->result[move->src]);
    }

    TEST(resolve_move_test, file_ambiguities)
    {
        auto board = Board::with_pieces({
                {"B4", Rook(Colour::white)},
                {"F4", Rook(Colour::white)}
        });

        auto sanmove = sanmove_with("D4", SquareType::rook);
        sanmove.src_x = file('B');
        auto move = resolve_move(sanmove, board);
        ASSERT_TRUE(move);
        EXPECT_EQ("D4", move->dest);
        EXPECT_EQ(Empty(), move->result[move->src]);
        EXPECT_EQ(Rook(Colour::white), move->result[move->dest]);

        auto ambiguous = sanmove_with("D4", SquareType::rook);
        EXPECT_FALSE(resolve_move(ambiguous, board));
    }

    TEST(resolve_move_test, rank_ambiguities)
    {
        auto board = Board::with_pieces({
                {"D7", Rook(Colour::white)},
                {"D1", Rook(Colour::white)}
        });

        auto sanmove = sanmove_with("D4", SquareType::rook);
        sanmove.src_y = rank(7);
        auto move = resolve_move(sanmove, board);
        ASSERT_TRUE(move);
        EXPECT_EQ("D4", move->dest);
        EXPECT_EQ(Empty(), move->result[move->src]);
        EXPECT_EQ(Rook(Colour::white), move->result[move->dest]);

        auto ambiguous = sanmove_with("D4", SquareType::rook);
        EXPECT_FALSE(resolve_move(ambiguous, board));
    }

    TEST(resolve_move_test, rank_and_file_ambiguities)
    {
        auto board = Board::with_pieces({
                {"D7", Rook(Colour::white)},
                {"D1", Rook(Colour::white)},
                {"A4", Rook(Colour::white)},
        });

        auto ambiguous_rook_on_d = sanmove_with("D4", SquareType::rook);
        ambiguous_rook_on_d.src_x = file('D');
        EXPECT_FALSE(resolve_move(ambiguous_rook_on_d, board));

        auto unambiguous_rook_on_a = sanmove_with("D4", SquareType::rook);
        unambiguous_rook_on_a.src_y = rank(4);
        EXPECT_TRUE(resolve_move(unambiguous_rook_on_a, board));

        auto unambiguous_rook_on_d7 = sanmove_with("D4", SquareType::rook);
        unambiguous_rook_on_d7.src_x = file('D');
        unambiguous_rook_on_d7.src_y = rank(7);
        EXPECT_TRUE(resolve_move(unambiguous_rook_on_d7, board));
    }

    TEST(resolve_move_test, incorrect_capture)
    {
        auto board = Board::with_pieces({
                {"D4", Rook(Colour::white)}
        });

        auto capturing = sanmove_with("D5", SquareType::rook);
        capturing.capture = true;
        EXPECT_FALSE(resolve_move(capturing, board));

        auto noncapturing = sanmove_with("D5", SquareType::rook);
        EXPECT_TRUE(resolve_move(noncapturing, board));
    }

    TEST(resolve_move_test, correct_capture)
    {
        auto board = Board::with_pieces({
                {"D4", Rook(Colour::white)},
                {"D5", Rook(Colour::black)}
        });

        auto capturing = sanmove_with("D5", SquareType::rook);
        capturing.capture = true;
        EXPECT_TRUE(resolve_move(capturing, board));

        // NOTE: Some PGN might not specify a capture, which would break given this assertion.
        auto noncapturing = sanmove_with("D5", SquareType::rook);
        EXPECT_FALSE(resolve_move(noncapturing, board));
    }

    TEST(resolve_move_test, incorrect_check)
    {
        auto board = Board::with_pieces({
                {"D4", Rook(Colour::white)},
                {"A5", King(Colour::black)}
        });

        auto checking = sanmove_with("B4", SquareType::rook);
        checking.check = true;
        EXPECT_FALSE(resolve_move(checking, board));

        auto not_checking = sanmove_with("B4", SquareType::rook);
        EXPECT_TRUE(resolve_move(not_checking, board));
    }

    TEST(resolve_move_test, correct_check)
    {
        auto board = Board::with_pieces({
                {"D4", Rook(Colour::white)},
                {"A5", King(Colour::black)}
        });

        auto checking = sanmove_with("A4", SquareType::rook);
        checking.check = true;
        EXPECT_TRUE(resolve_move(checking, board));

        auto not_checking = sanmove_with("A4", SquareType::rook);
        EXPECT_FALSE(resolve_move(not_checking, board));
    }

    TEST(resolve_move_test, checkmate)
    {
        auto board = Board::with_pieces({
                {"D7", Rook(Colour::white)},
                {"B1", Rook(Colour::white)},
                {"A5", King(Colour::black)}
        });

        auto false_mate = sanmove_with("D8", SquareType::rook);
        false_mate.checkmate = true;
        EXPECT_FALSE(resolve_move(false_mate, board));

        auto no_mate = sanmove_with("D8", SquareType::rook);
        EXPECT_TRUE(resolve_move(no_mate, board));

        auto true_mate = sanmove_with("A7", SquareType::rook);
        true_mate.checkmate = true;
        EXPECT_TRUE(resolve_move(true_mate, board));
    }

    TEST(resolve_move_test, kingside_castle_white)
    {
        auto board = Board::with_pieces({
                {"H1", Rook(Colour::white)},
                {"E1", King(Colour::white)}
        });

        auto kingside = SanMove{};
        kingside.king_side_castle = true;
        EXPECT_TRUE(resolve_move(kingside, board));

        auto not_kingside = SanMove{};
        EXPECT_FALSE(resolve_move(not_kingside, board));
    }

    TEST(resolve_move_test, kingside_castle_black)
    {
        auto board = Board::with_pieces({
                {"H8", Rook(Colour::black)},
                {"E8", King(Colour::black)}
        });
        board.turn = Colour::black;

        auto kingside = SanMove{};
        kingside.king_side_castle = true;
        EXPECT_TRUE(resolve_move(kingside, board));

        auto not_kingside = SanMove{};
        EXPECT_FALSE(resolve_move(not_kingside, board));
    }

    TEST(resolve_move_test, queenside_castle_white)
    {
        auto board = Board::with_pieces({
                {"A1", Rook(Colour::white)},
                {"E1", King(Colour::white)}
        });

        auto queenside = SanMove{};
        queenside.queen_side_castle = true;
        EXPECT_TRUE(resolve_move(queenside, board));

        auto not_queenside = SanMove{};
        EXPECT_FALSE(resolve_move(not_queenside, board));
    }

    TEST(resolve_move_test, queenside_castle_black)
    {
        auto board = Board::with_pieces({
                {"A8", Rook(Colour::black)},
                {"E8", King(Colour::black)}
        });
        board.turn = Colour::black;

        auto queenside = SanMove{};
        queenside.queen_side_castle = true;
        EXPECT_TRUE(resolve_move(queenside, board));

        auto not_queenside = SanMove{};
        EXPECT_FALSE(resolve_move(not_queenside, board));
    }

    TEST(resolve_move_test, promotion_white)
    {
        auto board = Board::with_pieces({
                {"A7", Pawn(Colour::white)},
        });

        auto promoting_pawn = sanmove_with("A8", SquareType::pawn);
        promoting_pawn.promotion = SquareType::queen;
        EXPECT_TRUE(resolve_move(promoting_pawn, board));
    }

    TEST(resolve_move_test, promotion_black)
    {
        auto board = Board::with_pieces({
                {"A2", Pawn(Colour::black)},
        });
        board.turn = Colour::black;

        auto promoting_pawn = sanmove_with("A1", SquareType::pawn);
        promoting_pawn.promotion = SquareType::queen;
        EXPECT_TRUE(resolve_move(promoting_pawn, board));
    }

    TEST(resolve_move_test, en_passant_should_be_capture)
    {
        auto driver = BasicDriver{};
        auto start = Board::with_pieces({
                {"A2", Pawn(Colour::white)},
                {"B4", Pawn(Colour::black)}
        });
        auto game = Game{driver, start};
        game.move("A2", "A4");

        auto en_passant = sanmove_with("A3", SquareType::pawn);
        en_passant.src_x = file('B');
        en_passant.capture = true;
        EXPECT_TRUE(resolve_move(en_passant, game.board()));
    }

    TEST(resolve_move_test, promote_and_check)
    {
        auto driver = BasicDriver{};
        auto start = Board::with_pieces({
                {"A8", King(Colour::black)},
                {"D7", Pawn(Colour::white)}
        });
        auto game = Game{driver, start};

        auto san = sanmove_with("D8", SquareType::pawn);
        san.check = true;
        san.promotion = SquareType::queen;

        EXPECT_TRUE(resolve_move(san, game.board()));
    }

    TEST(resolve_move_test, promotion_next_to_similar_piece)
    {
        auto board =  Board::with_pieces({
                {"A7", Pawn(Colour::white)},
                {"B8", Queen(Colour::white)}
        });

        auto san = sanmove_with("A8", SquareType::pawn);
        san.promotion = SquareType::queen;

        EXPECT_TRUE(resolve_move(san, board));
    }

    TEST(resolve_move_test, stalemate)
    {
        auto board =  Board::with_pieces({
                {"A8", King(Colour::black)},
                {"A7", Pawn(Colour::white)},
                {"B5", King(Colour::white)}
        });

        auto san = sanmove_with("B6", SquareType::king);
        EXPECT_TRUE(resolve_move(san, board));
    }
}