#include "fuse_adaptor.h"
#include "common/to_do.h"
#include "trees/directory.h"
#include "trees/regular_file.h"
#include <algorithm>
#include <cstring>

namespace dogbox::fuse
{
    namespace
    {
        struct path_split_result
        {
            std::filesystem::path head;
            std::filesystem::path tail;
        };

        path_split_result split_path(std::filesystem::path const &original)
        {
            auto const &original_string = original.string();
            auto const slash = std::find(original_string.begin(), original_string.end(), '/');
            if (slash == original_string.end())
            {
                return path_split_result{original_string, ""};
            }
            return path_split_result{std::filesystem::path(original_string.begin(), slash),
                                     std::filesystem::path(slash + 1, original_string.end())};
        }

        struct directory_entry
        {
            tree::entry_type type;
            blob_hash_code hash_code;
        };

        std::optional<tree::decoded_entry> find_entry(std::byte const *begin, std::byte const *end,
                                                      std::string_view const entry_name)
        {
            std::byte const *cursor = begin;
            while (cursor != end)
            {
                std::optional<std::tuple<tree::decoded_entry, std::byte const *>> const decode_result =
                    tree::decode_entry(cursor, end);
                if (!decode_result)
                {
                    TO_DO();
                }
                tree::decoded_entry const &entry = std::get<0>(*decode_result);
                if (entry.name == entry_name)
                {
                    return entry;
                }
                cursor = std::get<1>(*decode_result);
            }
            return std::nullopt;
        }

        std::optional<directory_entry> resolve_path(sqlite3 &database, blob_hash_code const root,
                                                    std::filesystem::path const &resolving)
        {
            path_split_result const split = split_path(resolving);
            if (split.head.empty())
            {
                return directory_entry{tree::entry_type::directory, root};
            }
            std::optional<std::vector<std::byte>> const root_blob = load_blob(database, root);
            if (!root_blob)
            {
                TO_DO();
            }
            std::byte const *const begin = root_blob->data();
            std::byte const *const end = begin + root_blob->size();
            std::optional<tree::decoded_entry> const found_entry = find_entry(begin, end, split.head.string());
            if (!found_entry)
            {
                TO_DO();
            }
            tree::decoded_entry const &entry = *found_entry;
            switch (entry.type)
            {
            case tree::entry_type::directory:
                return resolve_path(database, entry.hash_code, split.tail);

            case tree::entry_type::regular_file:
                if (split.tail.empty())
                {
                    return directory_entry{tree::entry_type::regular_file, entry.hash_code};
                }
                return std::nullopt;
            }
            TO_DO();
        }

        struct stat directory_entry_to_stat(directory_entry const entry)
        {
            struct stat status = {};
            switch (entry.type)
            {
            case tree::entry_type::regular_file:
                status.st_mode = S_IFREG | 0444;
                status.st_nlink = 1;
                status.st_size = 1234 /*TODO*/;
                break;

            case tree::entry_type::directory:
                status.st_mode = S_IFDIR | 0755;
                status.st_nlink = 2;
                break;
            }
            return status;
        }

        int adaptor_getattr(const char *const request_path, struct stat *const into)
        {
            fuse_context &fuse = *fuse_get_context();
            user_data &user = *static_cast<user_data *>(fuse.private_data);
            std::optional<directory_entry> const resolved = resolve_path(user.database, user.root, request_path + 1);
            if (!resolved)
            {
                return -ENOENT;
            }
            *into = directory_entry_to_stat(*resolved);
            return 0;
        }

        int adaptor_readdir(const char *request_path, void *buf, fuse_fill_dir_t filler, off_t offset,
                            struct fuse_file_info *file)
        {
            // TODO: use offset
            (void)offset;
            (void)file;

            fuse_context &fuse = *fuse_get_context();
            user_data &user = *static_cast<user_data *>(fuse.private_data);
            std::optional<directory_entry> const resolved = resolve_path(user.database, user.root, request_path + 1);
            if (!resolved)
            {
                return -ENOENT;
            }

            switch (resolved->type)
            {
            case tree::entry_type::directory:
            {
                std::optional<std::vector<std::byte>> const root_blob = load_blob(user.database, resolved->hash_code);
                if (!root_blob)
                {
                    TO_DO();
                }
                std::byte const *const begin = root_blob->data();
                std::byte const *const end = begin + root_blob->size();
                std::byte const *cursor = begin;
                while (cursor != end)
                {
                    std::optional<std::tuple<tree::decoded_entry, std::byte const *>> const decode_result =
                        tree::decode_entry(cursor, end);
                    if (!decode_result)
                    {
                        TO_DO();
                    }
                    tree::decoded_entry const &entry = std::get<0>(*decode_result);
                    struct stat const status = directory_entry_to_stat(directory_entry{entry.type, entry.hash_code});
                    filler(buf, std::string(entry.name).c_str(), &status, 0 /*TODO use offset*/);
                    cursor = std::get<1>(*decode_result);
                }
                return 0;
            }

            case tree::entry_type::regular_file:
                TO_DO();
            }
            TO_DO();
        }

        int adaptor_open(const char *request_path, struct fuse_file_info *file)
        {
            fuse_context &fuse = *fuse_get_context();
            user_data &user = *static_cast<user_data *>(fuse.private_data);
            std::optional<directory_entry> const resolved = resolve_path(user.database, user.root, request_path + 1);
            if (!resolved)
            {
                return -ENOENT;
            }
            switch (resolved->type)
            {
            case tree::entry_type::directory:
                return -EISDIR;

            case tree::entry_type::regular_file:
                for (size_t i = 0; i < user.files.size(); ++i)
                {
                    auto &entry = user.files[i];
                    if (!entry)
                    {
                        entry = open_file{resolved->hash_code, std::nullopt};
                        file->fh = i;
                        return 0;
                    }
                }
                file->fh = user.files.size();
                user.files.emplace_back(open_file{resolved->hash_code, std::nullopt});
                return 0;
            }
            TO_DO();
        }

        int adaptor_release(const char *request_path, struct fuse_file_info *file)
        {
            (void)request_path;
            fuse_context &fuse = *fuse_get_context();
            user_data &user = *static_cast<user_data *>(fuse.private_data);
            user.files[static_cast<size_t>(file->fh)] = std::nullopt;
            return 0;
        }

        regular_file_index load_regular_file_index(sqlite3 &database, blob_hash_code const hash_code)
        {
            std::optional<std::vector<std::byte>> const loaded = load_blob(database, hash_code);
            if (!loaded)
            {
                TO_DO();
            }
            std::byte const *const begin = loaded->data();
            std::byte const *const end = begin + loaded->size();
            std::optional<std::tuple<regular_file::length_type, std::byte const *>> const header =
                regular_file::start_decoding(begin, end);
            if (!header)
            {
                TO_DO();
            }
            regular_file::length_type const file_size = std::get<0>(*header);
            const size_t size = static_cast<size_t>(file_size / regular_file::piece_length);
            std::vector<blob_hash_code> pieces(size);
            std::byte const *cursor = std::get<1>(*header);
            for (size_t i = 0; i < pieces.size(); ++i)
            {
                std::optional<std::tuple<sha256_hash_code, std::byte const *>> const piece =
                    regular_file::decode_piece(cursor, end);
                if (!piece)
                {
                    TO_DO();
                }
                pieces.emplace_back(std::get<0>(*piece));
                cursor = std::get<1>(*piece);
            }
            regular_file::length_type const tail_size = (file_size - (pieces.size() * blob_hash_code().digits.size()));
            std::byte const *const tail = regular_file::finish_decoding(cursor, end, tail_size);
            if (!tail)
            {
                TO_DO();
            }
            return regular_file_index{std::move(pieces), std::vector<std::byte>(tail, tail + tail_size)};
        }

        int adaptor_read(const char *request_path, char *const into, size_t const size, off_t const offset,
                         struct fuse_file_info *const file)
        {
            (void)request_path;
            fuse_context &fuse = *fuse_get_context();
            user_data &user = *static_cast<user_data *>(fuse.private_data);
            assert(file->fh < user.files.size());
            open_file &opened = *user.files[static_cast<size_t>(file->fh)];
            if (!opened.index)
            {
                opened.index = load_regular_file_index(user.database, opened.hash_code);
            }
            assert(opened.index);
            size_t remaining_size = size;
            regular_file::length_type read_cursor = static_cast<regular_file::length_type>(offset);
            std::byte *write_cursor = reinterpret_cast<std::byte *>(into);
            while (remaining_size > 0)
            {
                size_t const current_piece_index = static_cast<size_t>(read_cursor / regular_file::piece_length);
                std::vector<std::byte> *piece = nullptr;
                std::optional<std::vector<std::byte>> loaded_piece;
                if (current_piece_index < opened.index->pieces.size())
                {
                    loaded_piece = load_blob(user.database, opened.index->pieces[current_piece_index]);
                    if (!loaded_piece)
                    {
                        TO_DO();
                    }
                    piece = &*loaded_piece;
                }
                else
                {
                    assert(opened.index);
                    piece = &opened.index->tail;
                }
                size_t const offset_in_piece = (read_cursor % regular_file::piece_length);
                if (offset_in_piece >= piece->size())
                {
                    break;
                }
                size_t const copying = std::min(piece->size(), remaining_size);
                std::memcpy(write_cursor, piece->data() + offset_in_piece, copying);
                write_cursor += copying;
                read_cursor += copying;
                remaining_size -= copying;
            }
            // TODO handle overflow
            return static_cast<int>(std::distance(reinterpret_cast<std::byte *>(into), write_cursor));
        }
    }

    fuse_operations make_operations() noexcept
    {
        fuse_operations result = {};
        result.getattr = adaptor_getattr;
        result.readdir = adaptor_readdir;
        result.open = adaptor_open;
        result.release = adaptor_release;
        result.read = adaptor_read;
        return result;
    }
}
