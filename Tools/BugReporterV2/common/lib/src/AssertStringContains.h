#pragma once

#include <UnitTest++.h>
#include <string>

namespace ureport
{
namespace test
{
    template<typename Char>
    void AssertStringContains(const std::basic_string<Char>& string,
        const std::basic_string<Char>& subString)
    {
        CHECK(string.find(subString) != std::basic_string<Char>::npos);
    }

    template<typename Char>
    void AssertStringContains(const std::basic_string<Char>& string, const Char* subString)
    {
        AssertStringContains(string, std::basic_string<Char>(subString));
    }
}
}
