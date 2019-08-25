#include "common/create_random_file.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

namespace
{
    constexpr uint64_t random_file_sizes[] = {0, 1000 * 100, 1000 * 1000};
}

BOOST_DATA_TEST_CASE(create_random_file, random_file_sizes, size)
{
    std::filesystem::path const file = "/tmp/dogbox_test_random_file";
    dogbox::create_random_file(file, size);
    BOOST_TEST(size == std::filesystem::file_size(file));
    std::filesystem::remove(file);
}
