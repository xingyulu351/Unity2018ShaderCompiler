#pragma once

#include <string>
#include <algorithm>
#include "shims/logical/IsEmpty.h"

namespace ureport
{
namespace path_utils
{
    inline
    std::string ConvertToSlashedPath(const std::string& path)
    {
        std::string res;
        std::for_each(path.cbegin(), path.cend(),
            [&res] (const char c) {
                if ('\\' == c)
                    res.push_back('/');
                else
                    res.push_back(c);
            });
        return res;
    }

    inline
    std::string GetBaseName(const std::string& path)
    {
        const auto slash = path.rfind(L'/');
        const auto backSlash = path.rfind(L'\\');
        auto start = std::string::npos == slash ? 0 : slash + 1;
        if (std::string::npos != backSlash)
            start = std::max(start, backSlash + 1);
        return path.substr(start);
    }

    inline
    std::string GetRelativePath(
        const std::string& path,
        const std::string& basePath)
    {
        auto canPath(ConvertToSlashedPath(path));
        auto canBasePath(ConvertToSlashedPath(basePath));
        if (canPath.length() == 0 || canBasePath.length() == 0)
            return "";
        if (canBasePath.compare(canPath.substr(0, canBasePath.length())))
            return "";
        auto res(path.substr(basePath.length()));
        if (res.length() > 0 && ('/' == res[0] || '\\' == res[0]))
            res.erase(0, 1);
        return res;
    }

    inline
    std::string GetSuffix(const std::string& path)
    {
        if (IsEmpty(path))
            return "";
        auto filenameStartPos = path.find_last_of('/');
        if (filenameStartPos == std::string::npos)
            filenameStartPos = 0;
        auto filename = path.substr(filenameStartPos);
        auto dotPos = filename.find_last_of('.');
        if (dotPos == std::string::npos)
            return "";
        return filename.substr(dotPos);
    }
}
}
