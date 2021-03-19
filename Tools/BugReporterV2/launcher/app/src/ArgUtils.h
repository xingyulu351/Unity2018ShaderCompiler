#pragma once

#include "shims/logical/IsEmpty.h"
#include "shims/attribute/GetSize.h"

#include <string>
#include <cstring>
#include <cwchar>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>

namespace ureport
{
namespace launcher
{
    inline
    bool IsFactSwitch(const std::string& string)
    {
        return string == "--fact";
    }

    inline
    std::map<std::string, std::string> GetFacts(const std::vector<std::string>& args)
    {
        std::map<std::string, std::string> facts;
        if (IsEmpty(args))
            return facts;
        if (IsFactSwitch(args[0]))
            facts[args[1]] = GetSize(args) > 2 ? args[2] : std::string();
        return facts;
    }

    inline
    bool IsUnityProjectSwitch(const std::string& string)
    {
        return string == "--unity_project";
    }

    inline
    std::string GetUnityProject(const std::vector<std::string>& args)
    {
        auto iter = std::find_if(args.begin(), args.end(), IsUnityProjectSwitch);
        if (iter != args.end())
            std::advance(iter, 1);
        return iter != args.end() ? *iter : std::string();
    }
}

    inline
    std::vector<std::string> MakeArgsVector(int argc, const char* argv[])
    {
        std::vector<std::string> args;
        std::copy(&argv[0], &argv[argc], std::back_inserter(args));
        return args;
    }
}
