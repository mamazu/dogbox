#pragma once

#include "blob_layer/hash_code.h"
#include <optional>
#include <tuple>

namespace dogbox
{
    namespace tree
    {
        enum class entry_type
        {
            regular_file = 254,
            directory = 255
        };

        inline std::ostream &operator<<(std::ostream &out, entry_type const printed)
        {
            switch (printed)
            {
            case entry_type::regular_file:
                return out << "regular_file";
            case entry_type::directory:
                return out << "directory";
            }
            return out << "???";
        }

        template <class ByteOutputIterator>
        void encode_entry(entry_type const type, std::string_view const name, sha256_hash_code const &hash_code,
                          ByteOutputIterator out)
        {
            *out++ = static_cast<std::byte>(type);
            std::byte const *const name_begin = reinterpret_cast<std::byte const *>(name.data());
            out = std::copy(name_begin, name_begin + name.size(), out);
            *out++ = static_cast<std::byte>(0);
            out = std::copy(hash_code.digits.begin(), hash_code.digits.end(), out);
        }

        struct decoded_entry
        {
            entry_type type;
            std::string_view name;
            sha256_hash_code hash_code;
        };

        inline bool operator==(decoded_entry const &left, decoded_entry const &right) noexcept
        {
            return std::tie(left.type, left.name, left.hash_code) == std::tie(right.type, right.name, right.hash_code);
        }

        inline std::ostream &operator<<(std::ostream &out, decoded_entry const &printed)
        {
            return out << printed.type << " " << printed.name << " " << printed.hash_code;
        }

        std::optional<std::tuple<decoded_entry, std::byte const *>> decode_entry(std::byte const *in,
                                                                                 std::byte const *const end);
    }
}
