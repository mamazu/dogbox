#include "common/byte_literal.h"
#include "trees/import.h"
#include <boost/test/unit_test.hpp>

namespace
{
    std::filesystem::path find_test_directories()
    {
        return std::filesystem::path(__FILE__).parent_path() / "test_directories";
    }
}

BOOST_AUTO_TEST_CASE(import_from_filesystem_regular_file)
{
    dogbox::sqlite_handle const database = dogbox::open_sqlite(":memory:");
    dogbox::initialize_blob_storage(*database);
    BOOST_TEST(dogbox::count_blobs(*database) == 0);
    dogbox::sha256_hash_code const test_txt_hash_code =
        dogbox::import::from_filesystem_regular_file(*database, find_test_directories() / "nested" / "test.txt");
    BOOST_TEST(
        dogbox::parse_sha256_hash_code("ead76f8e70b5dd3b1a07a92c25c425b2b27198728862103d65c31c621e52a6aa").value() ==
        test_txt_hash_code);
    BOOST_TEST(dogbox::count_blobs(*database) == 1);
    std::optional<std::vector<std::byte>> const content = dogbox::load_blob(*database, test_txt_hash_code);
    BOOST_REQUIRE(content);
    std::vector<std::byte> const &content_ref = *content;
    using namespace dogbox::literals;
    std::array<std::byte, 9> const expected = {0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 0_b, 1_b, static_cast<std::byte>('A')};
    BOOST_TEST(std::equal(expected.begin(), expected.end(), content_ref.begin(), content_ref.end()));
}

BOOST_AUTO_TEST_CASE(import_from_filesystem_directory)
{
    dogbox::sqlite_handle const database = dogbox::open_sqlite(":memory:");
    dogbox::initialize_blob_storage(*database);
    BOOST_TEST(dogbox::count_blobs(*database) == 0);
    dogbox::sha256_hash_code const test_txt_hash_code =
        dogbox::import::from_filesystem_directory(*database, find_test_directories() / "nested");
    BOOST_TEST(
        dogbox::parse_sha256_hash_code("ca9d81f5e465244e7df9adb5b99d3302e38f32d496640dde1577a274eec5800d").value() ==
        test_txt_hash_code);
    BOOST_TEST(dogbox::count_blobs(*database) == 6);
    std::optional<std::vector<std::byte>> const content = dogbox::load_blob(*database, test_txt_hash_code);
    BOOST_REQUIRE(content);
    // TODO check whether content is correct
}
