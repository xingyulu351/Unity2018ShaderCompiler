#pragma once

#include <memory>

namespace ureport
{
    template<typename Type>
    bool IsNull(const std::shared_ptr<Type>& ptr)
    {
        return ptr.get() == nullptr;
    }
}
