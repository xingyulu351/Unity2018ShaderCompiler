#pragma once

#include <cstddef>

namespace ureport
{
    template<typename C>
    size_t GetSize(const C& container)
    {
        return container.size();
    }
}
