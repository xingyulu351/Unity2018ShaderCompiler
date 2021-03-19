#pragma once

#include <string>
#include <algorithm>
#include <functional>
#include <cctype>

namespace testing
{
    inline
    std::string ToLower(const std::string& str)
    {
        std::string res(str);
        std::transform(str.begin(), str.end(), res.begin(),
            std::function<int(int)>(::tolower));
        return res;
    }
}
