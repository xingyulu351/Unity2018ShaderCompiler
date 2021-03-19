#pragma once

#include <string>
#include <cstdio>
#include <UnitTest++.h>

namespace ureport
{
namespace test
{
    inline
    void AssertFileExists(const std::string& path)
    {
        FILE* const file = std::fopen(path.c_str(), "r");
        std::fclose(file);
        CHECK(file != nullptr);
    }
}
}
