#include <chess/Board.h>
#include <chess/Suggester.h>

#include <benchmark/benchmark.h>

using chess::Board;
using chess::Suggester;

namespace
{
    void bench_suggester_standard_board(benchmark::State& state) {
        auto board = Board::standard();

        for (auto _ : state)
        {
            auto suggester = Suggester{board, chess::evaluate_with_summation};
            benchmark::DoNotOptimize(suggester.suggest());
        }
    }
}

BENCHMARK(bench_suggester_standard_board)->Unit(benchmark::kMillisecond);
BENCHMARK_MAIN();