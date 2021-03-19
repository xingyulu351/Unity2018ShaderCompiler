#pragma once

#include "shims/logical/IsEmpty.h"
#include "shims/attribute/GetSize.h"

#include <string>
#include <locale>
#include <vector>
#include <random>
#include <algorithm>

/*
 * codecvt and Widen()/Narrow() functions are only used on windows,
 * don't build them on linux
 */
/* TODO: replace the ifdefs with some proper windows only logic */
#ifndef LINUX
#include <codecvt>
#endif

namespace ureport
{
#ifndef LINUX
    static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> utf8conv;

    inline
    std::wstring Widen(const std::string& narrow)
    {
        return utf8conv.from_bytes(narrow.c_str());
    }

    inline
    std::string Narrow(const std::wstring& widen)
    {
        return utf8conv.to_bytes(widen.c_str());
    }

#endif

    template<typename Iterator>
    typename Iterator::value_type Join(Iterator begin, Iterator end,
        const typename Iterator::value_type& separator)
    {
        typename Iterator::value_type result;
        Iterator& iter = begin;
        if (iter == end)
            return result;
        result = *iter;
        for (++iter; iter != end; ++iter)
            if (*iter != typename Iterator::value_type())
            {
                if (result != typename Iterator::value_type())
                    result += separator;
                result += *iter;
            }
        return result;
    }

    inline
    std::string MakeRandomString(size_t length, const std::string& chars)
    {
        if (IsEmpty(chars))
            return std::string();
        std::string random(length, 0);
        static time_t time;
        static unsigned long seed = static_cast<unsigned long>(std::time(&time));
        static std::default_random_engine engine(seed);
        std::uniform_int_distribution<size_t> distr(0, GetSize(chars) - 1);
        std::generate_n(random.begin(), length, [&chars, &distr] ()
        {
            return chars[distr(engine)];
        });
        return random;
    }
}
