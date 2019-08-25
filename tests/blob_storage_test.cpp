#include "blob_layer/blob_storage.h"
#include "common/byte_literal.h"
#include <algorithm>
#include <boost/test/unit_test.hpp>

namespace
{
    struct sqlite_deleter
    {
        void operator()(sqlite3 *const database) const
        {
            sqlite3_close(database);
        }
    };

    using sqlite_handle = std::unique_ptr<sqlite3, sqlite_deleter>;

    sqlite_handle open_sqlite(char const *const name)
    {
        sqlite3 *database = nullptr;
        sqlite3_open(name, &database);
        return sqlite_handle(database);
    }
}

BOOST_AUTO_TEST_CASE(initialize_blob_storage)
{
    sqlite_handle const database = open_sqlite(":memory:");
    dogbox::initialize_blob_storage(*database);
}

BOOST_AUTO_TEST_CASE(load_blob_not_found)
{
    sqlite_handle const database = open_sqlite(":memory:");
    dogbox::initialize_blob_storage(*database);
    BOOST_TEST(!dogbox::load_blob(*database, dogbox::blob_hash_code{}));
}

BOOST_AUTO_TEST_CASE(store_blob)
{
    sqlite_handle const database = open_sqlite(":memory:");
    dogbox::initialize_blob_storage(*database);
    using namespace dogbox::literals;
    std::array<std::byte, 5> const test_blob = {1_b, 2_b, 3_b, 99_b, 255_b};
    dogbox::sha256_hash_code const hash = dogbox::store_blob(*database, test_blob.data(), test_blob.size());
    std::optional<std::vector<std::byte>> const loaded = dogbox::load_blob(*database, hash);
    BOOST_REQUIRE(loaded);
    BOOST_REQUIRE(loaded->size() == test_blob.size());
    BOOST_TEST(std::equal(test_blob.begin(), test_blob.end(), loaded->begin()));
    BOOST_TEST(!dogbox::load_blob(*database, dogbox::blob_hash_code{}));
}

BOOST_AUTO_TEST_CASE(store_blob_again)
{
    sqlite_handle const database = open_sqlite(":memory:");
    dogbox::initialize_blob_storage(*database);
    using namespace dogbox::literals;
    std::array<std::byte, 5> const test_blob = {1_b, 2_b, 3_b, 99_b, 255_b};
    dogbox::sha256_hash_code const hash_0 = dogbox::store_blob(*database, test_blob.data(), test_blob.size());
    boost::ignore_unused(hash_0);
    BOOST_CHECK_EXCEPTION(dogbox::store_blob(*database, test_blob.data(), test_blob.size()), dogbox::sqlite_error,
                          [](dogbox::sqlite_error const &error) {
                              return error.what() == std::string("SQLite operation failed with: 1; SQL logic error");
                          });
}
