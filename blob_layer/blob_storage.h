#pragma once

#include "hash_code.h"
#include "sqlite3.h"
#include <cassert>
#include <optional>
#include <vector>

namespace dogbox
{
    struct sqlite_error : std::runtime_error
    {
        explicit sqlite_error(int error_code, char const *const message)
            : std::runtime_error("SQLite operation failed with: " + std::to_string(error_code) + "; " + message)
        {
        }
    };

    using blob_hash_code = sha256_hash_code;

    void initialize_blob_storage(sqlite3 &database);

    blob_hash_code store_blob(sqlite3 &database, std::byte const *const data, size_t const size);

    std::optional<std::vector<std::byte>> load_blob(sqlite3 &database, blob_hash_code const hash_code);
}
