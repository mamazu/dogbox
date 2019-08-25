#include "blob_layer/blob_storage.h"
#include "common/byte_literal.h"
#include <algorithm>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(initialize_blob_storage)
{
    dogbox::sqlite_handle const database = dogbox::open_sqlite(":memory:");
    dogbox::initialize_blob_storage(*database);
}

BOOST_AUTO_TEST_CASE(load_blob_not_found)
{
    dogbox::sqlite_handle const database = dogbox::open_sqlite(":memory:");
    dogbox::initialize_blob_storage(*database);
    BOOST_TEST(!dogbox::load_blob(*database, dogbox::blob_hash_code{}));
}

BOOST_AUTO_TEST_CASE(store_blob)
{
    dogbox::sqlite_handle const database = dogbox::open_sqlite(":memory:");
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
    dogbox::sqlite_handle const database = dogbox::open_sqlite(":memory:");
    dogbox::initialize_blob_storage(*database);
    using namespace dogbox::literals;
    std::array<std::byte, 5> const test_blob = {1_b, 2_b, 3_b, 99_b, 255_b};
    dogbox::sha256_hash_code const hash_0 = dogbox::store_blob(*database, test_blob.data(), test_blob.size());
    dogbox::sha256_hash_code const hash_1 = dogbox::store_blob(*database, test_blob.data(), test_blob.size());
    BOOST_TEST(hash_0 == hash_1);
}
