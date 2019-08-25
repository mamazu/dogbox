#include "in_memory_fuse_frontend_shared/directory.h"
#include "trees/import.h"
#include <benchmark/benchmark.h>
#include <random>

static void benchmark_scan_directory(benchmark::State &state)
{
    for (auto _ : state)
    {
        dogbox::scan_directory(std::filesystem::path(__FILE__).parent_path().parent_path());
    }
}

BENCHMARK(benchmark_scan_directory)->Unit(benchmark::kMillisecond);

static void benchmark_import(benchmark::State &state)
{
    using random_bytes_engine = std::independent_bits_engine<std::mt19937, 8, unsigned char>;
    random_bytes_engine rbe;
    std::vector<unsigned char> blob(1024 * 1024 * 10);
    std::generate(begin(blob), end(blob), std::ref(rbe));
    for (auto _ : state)
    {
        dogbox::sqlite_handle const database = dogbox::open_sqlite(":memory:");
        dogbox::initialize_blob_storage(*database);
        dogbox::store_blob(*database, reinterpret_cast<std::byte const *>(blob.data()), blob.size());
    }
}

BENCHMARK(benchmark_import)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
