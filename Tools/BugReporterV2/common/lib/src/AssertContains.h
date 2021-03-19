#pragma once

#include <UnitTest++.h>
#include <algorithm>

namespace ureport
{
namespace test
{
    template<typename Container, typename Element>
    void AssertContains(const Container& container, const Element& element)
    {
        CHECK(std::find(container.begin(), container.end(), element) != container.end());
    }
}
}
