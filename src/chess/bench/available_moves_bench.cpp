#include <chess/Board.h>
#include <chess/available_moves.h>

#include <chess/pgn/MoveParser.h>
#include <chess/pgn/resolve_move.h>

#include <benchmark/benchmark.h>

#include <sstream>

using chess::Board;
using chess::available_moves;
using chess::Pawn;
using chess::Colour;
using chess::pgn::MoveParser;

namespace
{
    void bench_available_moves_one_pawn(benchmark::State& state)
    {
        auto board = Board::with_pieces({
                {"C2", Pawn(Colour::white)}
        });

        for (auto _ : state)
        {
            benchmark::DoNotOptimize(available_moves(board));
        }
    }

    void bench_available_moves_standard_board(benchmark::State& state)
    {
        auto board = Board::standard();
        for (auto _ : state)
        {
            benchmark::DoNotOptimize(available_moves(board));
        }
    }

    void bench_available_moves_for_some_pgn(benchmark::State& state)
    {
        auto stream = std::istringstream{R"(
            [Event "FICS rated standard game"]
            [Site "FICS freechess.org"]
            [FICSGamesDBGameNo "437661062"]
            [White "Miclez"]
            [Black "draganazdejkovic"]
            [WhiteElo "1993"]
            [BlackElo "2155"]
            [WhiteRD "76.4"]
            [BlackRD "257.4"]
            [TimeControl "1200+10"]
            [Date "2018.12.30"]
            [Time "06:56:00"]
            [WhiteClock "0:20:00.000"]
            [BlackClock "0:20:00.000"]
            [ECO "D13"]
            [PlyCount "85"]
            [Result "1-0"]

            1. d4 c6 2. c4 d5 3. Nc3 Nf6 4. Nf3 a6 5. cxd5 cxd5 6. e3 Nc6 7. Be2 Bg4 8. O-O e6 9. b3 Bd6 10. Bb2 Rc8
            11. Rc1 O-O 12. a3 b5 13. b4 Bb8 14. Ne1 Qd6 15. g3 Bh3 16. Ng2 Nd7 17. Bd3 Nb6 18. Qh5 Bf5 19. Bxf5 exf5
            20. Qxf5 Nc4 21. Rc2 Rfd8 22. Rfc1 g6 23. Qf3 Ne7 24. Nf4 Qd7 25. Nd3 Qc6 26. Nc5 Bd6 27. h4 Bxc5
            28. dxc5 Qe6 29. Rd1 Rd7 30. Ne2 Nxb2 31. Rxb2 Nf5 32. Rbd2 Rcd8 33. Nf4 Qe5 34. Rxd5 Rxd5 35. Rxd5 Rxd5
            36. Qxd5 Qa1+ 37. Kg2 Qxa3 38. e4 Nh6 39. c6 Qc3 40. Qd8+ Kg7 41. c7 Qc2 42. c8=Q Qxe4+
            43. Kh2 {Black resigns} 1-0
        )"};

        auto moves = MoveParser{stream}.next_game();

        for (auto _ : state)
        {
            auto board = Board::standard();

            for (auto const& san : *moves)
            {
                auto move = chess::pgn::resolve_move(san, board);
                if (!move)
                {
                    throw std::logic_error{"Game ended prematurely"};
                }

                board = move->result;
                benchmark::DoNotOptimize(board);
            }
        }
    }
}

BENCHMARK(bench_available_moves_one_pawn)->Unit(benchmark::kMicrosecond);
BENCHMARK(bench_available_moves_standard_board)->Unit(benchmark::kMicrosecond);
BENCHMARK(bench_available_moves_for_some_pgn)->Unit(benchmark::kMicrosecond);
BENCHMARK_MAIN();