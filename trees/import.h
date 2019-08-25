#pragma once

#include "blob_layer/blob_storage.h"
#include <filesystem>

namespace dogbox::import
{
    enum class parallelism
    {
        none,
        full
    };

    std::ostream &operator<<(std::ostream &out, parallelism const printed);

    blob_hash_code from_filesystem_directory(sqlite3 &database, std::filesystem::path const &root,
                                             parallelism const parallel);

    blob_hash_code from_filesystem_regular_file(sqlite3 &database, std::filesystem::path const &input,
                                                parallelism const parallel);
}
