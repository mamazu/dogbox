#include <benchmark/benchmark.h>
#include <boost/beast/http.hpp>
#include <sqlite3.h>

static void SomeFunction()
{
    sqlite3 *db;
    sqlite3_stmt *res;

    int rc = sqlite3_open(":memory:", &db);

    if (rc != SQLITE_OK)
    {

        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return;
    }

    rc = sqlite3_prepare_v2(db, "SELECT SQLITE_VERSION()", -1, &res, 0);

    if (rc != SQLITE_OK)
    {

        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return;
    }

    rc = sqlite3_step(res);

    sqlite3_finalize(res);
    sqlite3_close(db);
}

static void BM_SomeFunction(benchmark::State &state)
{
    for (auto _ : state)
    {
        SomeFunction();
    }
}

BENCHMARK(BM_SomeFunction);

BENCHMARK_MAIN();
