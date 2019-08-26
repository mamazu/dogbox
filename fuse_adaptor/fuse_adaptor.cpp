#include "fuse_adaptor.h"
#include <cstring>

namespace dogbox::fuse
{
    fuse_operations make_operations() noexcept
    {
        fuse_operations result = {};
        return result;
    }
}
