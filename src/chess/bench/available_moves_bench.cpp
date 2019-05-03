#include <chess/Board.h>
#include <chess/available_moves.h>

#include <benchmark/benchmark.h>

using chess::Board;
using chess::available_moves;
using chess::Pawn;
using chess::Colour;

namespace
{
    void bench_available_moves_one_pawn(benchmark::State& state) {
        auto board = Board::with_pieces({
                {"C2", Pawn(Colour::white)}
        });

        for (auto _ : state)
        {
            benchmark::DoNotOptimize(available_moves(board));
        }
    }

    void bench_available_moves_standard_board(benchmark::State& state) {
        auto board = Board::standard();
        for (auto _ : state)
        {
            benchmark::DoNotOptimize(available_moves(board));
        }
    }
}

BENCHMARK(bench_available_moves_one_pawn)->Unit(benchmark::kMicrosecond);
BENCHMARK(bench_available_moves_standard_board)->Unit(benchmark::kMicrosecond);
BENCHMARK_MAIN();