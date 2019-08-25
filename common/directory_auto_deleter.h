#pragma once

#include <filesystem>

namespace dogbox
{
    struct directory_auto_deleter
    {
        std::filesystem::path deleting;

        ~directory_auto_deleter()
        {
            std::filesystem::remove_all(deleting);
        }
    };
}
