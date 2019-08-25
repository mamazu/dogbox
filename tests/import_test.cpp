#include "common/byte_literal.h"
#include "common/create_random_file.h"
#include "common/directory_auto_deleter.h"
#include "common/to_do.h"
#include "trees/import.h"
#include "trees/regular_file.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <fstream>

namespace
{
    std::filesystem::path find_test_directories()
    {
        return std::filesystem::path(__FILE__).parent_path() / "test_directories";
    }

    constexpr dogbox::import::parallelism all_parallelism_modes[] = {
        dogbox::import::parallelism::none, dogbox::import::parallelism::full};
}

BOOST_DATA_TEST_CASE(import_from_filesystem_regular_file, all_parallelism_modes, parallel)
{
    dogbox::sqlite_handle const database = dogbox::open_sqlite(":memory:");
    dogbox::initialize_blob_storage(*database);
    BOOST_TEST(dogbox::count_blobs(*database) == 0);
    dogbox::sha256_hash_code const test_txt_hash_code = dogbox::import::from_filesystem_regular_file(
        *database, find_test_directories() / "nested" / "test.txt", parallel);
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

BOOST_DATA_TEST_CASE(import_from_filesystem_directory_small, all_parallelism_modes, parallel)
{
    dogbox::sqlite_handle const database = dogbox::open_sqlite(":memory:");
    dogbox::initialize_blob_storage(*database);
    BOOST_TEST(dogbox::count_blobs(*database) == 0);
    dogbox::sha256_hash_code const test_txt_hash_code =
        dogbox::import::from_filesystem_directory(*database, find_test_directories() / "nested", parallel);
    BOOST_TEST(
        dogbox::parse_sha256_hash_code("ca9d81f5e465244e7df9adb5b99d3302e38f32d496640dde1577a274eec5800d").value() ==
        test_txt_hash_code);
    BOOST_TEST(dogbox::count_blobs(*database) == 6);
    std::optional<std::vector<std::byte>> const content = dogbox::load_blob(*database, test_txt_hash_code);
    BOOST_REQUIRE(content);
    // TODO check whether content is correct
}

BOOST_DATA_TEST_CASE(import_from_filesystem_directory_large, all_parallelism_modes, parallel)
{
    using random_bytes_engine = std::independent_bits_engine<std::mt19937, 8, unsigned char>;
    random_bytes_engine random;
    std::filesystem::path const imported_dir =
        std::filesystem::path("/tmp") / std::to_string(std::uniform_int_distribution<uint64_t>()(random));
    dogbox::directory_auto_deleter const imported_dir_deleter{imported_dir};
    std::filesystem::create_directory(imported_dir);
    size_t total_file_size = 0;
    for (size_t i = 0; i < 2; ++i)
    {
        size_t const file_size = dogbox::regular_file::piece_length * 12;
        dogbox::create_random_file((imported_dir / std::to_string(i)), file_size);
        total_file_size += file_size;
    }
    dogbox::sqlite_handle const database = dogbox::open_sqlite(":memory:");
    dogbox::initialize_blob_storage(*database);
    BOOST_TEST(dogbox::count_blobs(*database) == 0);
    dogbox::sha256_hash_code const hash_code =
        dogbox::import::from_filesystem_directory(*database, imported_dir, parallel);
    BOOST_TEST(dogbox::count_blobs(*database) == 27);
    std::optional<std::vector<std::byte>> const content = dogbox::load_blob(*database, hash_code);
    BOOST_REQUIRE(content);
    // TODO check whether content is correct
}
