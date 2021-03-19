#pragma once

namespace ureport
{
    template<typename Type>
    Type* GetRawPtr(Type* ptr)
    {
        return ptr;
    }

    template<typename Type>
    const Type* GetRawPtr(const Type* ptr)
    {
        return ptr;
    }
}
