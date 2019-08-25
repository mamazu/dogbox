#pragma once

#include <array>
#include <ostream>

namespace dogbox
{
    template <class CharOutputIterator>
    void byte_to_hex(std::byte const in, CharOutputIterator &&out)
    {
        char const *const digits = "0123456789abcdef";
        *out++ = digits[static_cast<unsigned>(in) >> 4];
        *out++ = digits[static_cast<unsigned>(in) & 0x0f];
    }

    template <class ByteInputRange, class CharOutputIterator>
    void format_bytes(ByteInputRange const &input, CharOutputIterator &&output)
    {
        for (std::byte const b : input)
        {
            byte_to_hex(b, output);
        }
    }

    struct sha256_hash_code
    {
        std::array<std::byte, 256 / 8> digits;
    };

    inline bool operator==(sha256_hash_code const &left, sha256_hash_code const &right) noexcept
    {
        return left.digits == right.digits;
    }

    inline std::ostream &operator<<(std::ostream &out, sha256_hash_code const &printed)
    {
        format_bytes(printed.digits, std::ostreambuf_iterator<char>(out));
        return out;
    }

    sha256_hash_code sha256(std::byte const *const data, size_t const size);
    std::string to_string(sha256_hash_code const &hash_code);
}
