#include "hash_code.h"
#include <openssl/sha.h>

namespace dogbox
{
    sha256_hash_code sha256(std::byte const *const data, size_t const size)
    {
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, data, size);
        sha256_hash_code result;
        SHA256_Final(reinterpret_cast<unsigned char *>(result.digits.data()), &sha256);
        return result;
    }

    std::string to_string(sha256_hash_code const &hash_code)
    {
        std::string result;
        format_bytes(hash_code.digits, std::back_inserter(result));
        return result;
    }

    namespace
    {
        std::optional<unsigned> decode_hex_digit(char const c)
        {
            switch (c)
            {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                return (c - '0');

            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                return (c - 'a' + 10);

            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                return (c - 'A' + 10);

            default:
                return std::nullopt;
            }
        }
    }

    std::optional<sha256_hash_code> parse_sha256_hash_code(std::string_view const input)
    {
        sha256_hash_code result;
        if (input.size() != (result.digits.size() * 2))
        {
            return std::nullopt;
        }
        for (size_t i = 0; i < result.digits.size(); ++i)
        {
            auto const high = decode_hex_digit(input[i * 2]);
            if (!high)
            {
                return std::nullopt;
            }
            auto const low = decode_hex_digit(input[i * 2 + 1]);
            if (!low)
            {
                return std::nullopt;
            }
            result.digits[i] = static_cast<std::byte>((*high << 4u) | *low);
        }
        return result;
    }
}
