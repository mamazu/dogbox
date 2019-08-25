#pragma once

#include "blob_layer/blob_storage.h"
#include <filesystem>

namespace dogbox::import
{
    blob_hash_code from_filesystem_directory(sqlite3 &database, std::filesystem::path const &root);

    blob_hash_code from_filesystem_regular_file(sqlite3 &database, std::filesystem::path const &input);
}
