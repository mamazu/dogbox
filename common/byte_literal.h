#pragma once

#include <cassert>
#include <cstddef>

namespace dogbox
{
    inline namespace literals
    {
        constexpr std::byte operator"" _b(unsigned long long value)
        {
            assert(value < 256);
            return static_cast<std::byte>(value);
        }
    }
}
