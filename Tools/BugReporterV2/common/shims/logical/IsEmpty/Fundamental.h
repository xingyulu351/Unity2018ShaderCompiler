#pragma once

#include <cstring>
#include <cwchar>

namespace ureport
{
    inline
    size_t GetLength(const char* const string)
    {
        return std::strlen(string);
    }

    inline
    size_t GetLength(const wchar_t* const string)
    {
        return std::wcslen(string);
    }

    template<typename Char>
    bool IsEmpty(const Char* const string)
    {
        return string == nullptr || GetLength(string) == 0;
    }
}
