#include "import.h"
#include "common/to_do.h"
#include "directory.h"
#include "regular_file.h"
#include <fstream>

namespace dogbox::import
{
    blob_hash_code from_filesystem_directory(sqlite3 &database, std::filesystem::path const &root)
    {
        // TODO: sort entries (for example by name)
        std::vector<std::byte> encoded;
        std::filesystem::directory_iterator i(root);
        for (; i != std::filesystem::directory_iterator(); ++i)
        {
            auto entry_path = i->path();
            auto entry_name = entry_path.filename();
            switch (i->status().type())
            {
            case std::filesystem::file_type::regular:
            {
                blob_hash_code const regular = from_filesystem_regular_file(database, entry_path);
                tree::encode_entry(
                    tree::entry_type::regular_file, entry_name.string(), regular, std::back_inserter(encoded));
                break;
            }

            case std::filesystem::file_type::directory:
            {
                blob_hash_code const subdirectory = from_filesystem_directory(database, entry_path);
                tree::encode_entry(
                    tree::entry_type::directory, entry_name.string(), subdirectory, std::back_inserter(encoded));
                break;
            }

            case std::filesystem::file_type::none:
            case std::filesystem::file_type::not_found:
            case std::filesystem::file_type::symlink:
            case std::filesystem::file_type::block:
            case std::filesystem::file_type::character:
            case std::filesystem::file_type::fifo:
            case std::filesystem::file_type::socket:
            case std::filesystem::file_type::unknown:
                break;
            }
        }

        return store_blob(database, encoded.data(), encoded.size());
    }

    blob_hash_code from_filesystem_regular_file(sqlite3 &database, std::filesystem::path const &input)
    {
        std::ifstream file(input.string(), std::ios::binary);
        if (!file)
        {
            TO_DO();
        }
        auto const at_beginning = file.tellg();
        file.seekg(0, std::ios::end);
        auto const at_end = file.tellg();
        file.seekg(0, std::ios::beg);
        auto const size = static_cast<uint64_t>(at_end - at_beginning);
        std::vector<std::byte> encoded;
        regular_file::start_encoding(size, std::back_inserter(encoded));
        uint64_t remaining_size = size;
        std::vector<std::byte> piece_buffer(regular_file::piece_length);
        while (remaining_size >= piece_buffer.size())
        {
            file.read(reinterpret_cast<char *>(piece_buffer.data()), piece_buffer.size());
            if (!file)
            {
                TO_DO();
            }
            auto const piece_hash_code = store_blob(database, piece_buffer.data(), piece_buffer.size());
            regular_file::encode_piece(piece_hash_code, std::back_inserter(encoded));
            remaining_size -= piece_buffer.size();
        }
        file.read(reinterpret_cast<char *>(piece_buffer.data()), remaining_size);
        if (!file)
        {
            TO_DO();
        }
        regular_file::finish_encoding(piece_buffer.data(), remaining_size, std::back_inserter(encoded));
        return store_blob(database, encoded.data(), encoded.size());
    }
}
