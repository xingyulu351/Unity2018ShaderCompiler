#pragma once

namespace ureport
{
    template<typename Type>
    bool IsNull(const Type& value)
    {
        return 0 == value;
    }
}
