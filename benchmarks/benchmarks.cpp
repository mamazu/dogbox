#include "common/to_do.h"
#include "in_memory_fuse_frontend_shared/directory.h"
#include "trees/import.h"
#include <benchmark/benchmark.h>
#include <fstream>
#include <random>

static void benchmark_scan_directory(benchmark::State &state)
{
    for (auto _ : state)
    {
        dogbox::scan_directory(std::filesystem::path(__FILE__).parent_path().parent_path());
    }
}

BENCHMARK(benchmark_scan_directory)->Unit(benchmark::kMillisecond);

static void benchmark_store_blob(benchmark::State &state)
{
    using random_bytes_engine = std::independent_bits_engine<std::mt19937, 8, unsigned char>;
    random_bytes_engine rbe;
    std::vector<unsigned char> blob(1024 * 1024 * 2);
    std::generate(begin(blob), end(blob), std::ref(rbe));
    for (auto _ : state)
    {
        dogbox::sqlite_handle const database = dogbox::open_sqlite(":memory:");
        dogbox::initialize_blob_storage(*database);
        dogbox::store_blob(*database, reinterpret_cast<std::byte const *>(blob.data()), blob.size());
    }
    state.SetBytesProcessed(state.iterations() * blob.size());
}

BENCHMARK(benchmark_store_blob)->Unit(benchmark::kMillisecond);

static void benchmark_sha256(benchmark::State &state)
{
    using random_bytes_engine = std::independent_bits_engine<std::mt19937, 8, unsigned char>;
    random_bytes_engine rbe;
    std::vector<unsigned char> blob(static_cast<size_t>(state.range(0)));
    std::generate(begin(blob), end(blob), std::ref(rbe));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(dogbox::sha256(reinterpret_cast<std::byte const *>(blob.data()), blob.size()));
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(state.iterations() * blob.size());
}

BENCHMARK(benchmark_sha256)->Unit(benchmark::kMillisecond)->Range(0, 1024 * 1024 * 10);

struct directory_auto_deleter
{
    std::filesystem::path deleting;

    ~directory_auto_deleter()
    {
        std::filesystem::remove_all(deleting);
    }
};

static void benchmark_import_directory(benchmark::State &state)
{
    using random_bytes_engine = std::independent_bits_engine<std::mt19937, 8, unsigned char>;
    random_bytes_engine random;
    std::filesystem::path const imported_dir =
        std::filesystem::path("/tmp") / std::to_string(std::uniform_int_distribution<uint64_t>()(random));
    directory_auto_deleter const imported_dir_deleter{imported_dir};
    std::filesystem::create_directory(imported_dir);
    size_t total_file_size = 0;
    for (size_t i = 0; i < static_cast<size_t>(state.range(0)); ++i)
    {
        std::ofstream file((imported_dir / std::to_string(i)).string(), std::ios::binary);
        size_t const file_size = 777ull * (1 << i);
        total_file_size += file_size;
        std::generate_n(std::ostreambuf_iterator<char>(file), file_size, std::ref(random));
        if (!file)
        {
            TO_DO();
        }
    }
    for (auto _ : state)
    {
        dogbox::sqlite_handle const database = dogbox::open_sqlite(":memory:");
        dogbox::initialize_blob_storage(*database);
        dogbox::import::from_filesystem_directory(*database, imported_dir);
    }
    state.SetBytesProcessed(state.iterations() * total_file_size);
}

BENCHMARK(benchmark_import_directory)->Unit(benchmark::kMillisecond)->DenseRange(11, 17);

BENCHMARK_MAIN();
