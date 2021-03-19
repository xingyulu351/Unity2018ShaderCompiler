#pragma once

namespace ureport
{
    template<typename C>
    bool IsEmpty(const C& container)
    {
        return container.empty();
    }
}
