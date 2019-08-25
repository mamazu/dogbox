#include "in_memory_fuse_frontend_shared/directory.h"
#include <benchmark/benchmark.h>

static void benchmark_scan_directory(benchmark::State &state)
{
    for (auto _ : state)
    {
        dogbox::scan_directory(std::filesystem::path(__FILE__).parent_path().parent_path());
    }
}

BENCHMARK(benchmark_scan_directory)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
