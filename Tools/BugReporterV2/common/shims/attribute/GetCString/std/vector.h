#pragma once

#include <vector>

namespace ureport
{
    template<typename Char>
    const Char* GetCString(const std::vector<Char>& src)
    {
        return &src[0];
    }
}
