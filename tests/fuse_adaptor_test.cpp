#include "fuse_adaptor/fuse_adaptor.h"
#include <boost/test/unit_test.hpp>
#include <future>

BOOST_AUTO_TEST_CASE(fuse_adaptor)
{
    fuse_args arguments = {};
    std::filesystem::path const mount_point = "/tmp/dogbox_test_fuse_mount";
    std::filesystem::create_directories(mount_point);

    // fuse_unmount has to be called before fuse_destroy. That's why the destruction order of these handles is
    // important.
    std::unique_ptr<struct fuse, dogbox::fuse::fuse_deleter> fuse_handle;

    dogbox::fuse::channel const channel(
        [&]() -> fuse_chan & {
            fuse_chan *const result = fuse_mount(mount_point.c_str(), &arguments);
            BOOST_REQUIRE(result);
            return *result;
        }(),
        mount_point);
    auto const operations = dogbox::fuse::make_operations();
    auto user_data = dogbox::fuse::user_data();
    fuse_handle.reset(fuse_new(channel.handle, &arguments, &operations, sizeof(operations), &user_data));
    BOOST_REQUIRE(fuse_handle);

    auto worker = std::async(std::launch::async, [&]() { BOOST_REQUIRE(0 == fuse_loop(fuse_handle.get())); });

    fuse_exit(fuse_handle.get());
    worker.get();
}
