#pragma once

#include <string>
#include <functional>
#include <cstdlib>
#include "common/StringUtils.h"

namespace ureport
{
namespace unity_logs_utils
{
    inline
    std::string GetLogsLocationOnLinux(
        std::function<const char* (const char*)> getVar)
    {
        auto var(getVar("XDG_CONFIG_HOME"));
        if (var)
            return std::string(var) + "/";

        var = getVar("HOME");
        if (nullptr == var)
            return "";

        return std::string(var) + "/.config/";
    }

    inline
    std::string GetLogsLocationOnMacOSX(
        std::function<const char* (const char*)> getVar)
    {
        auto var(getVar("HOME"));
        if (nullptr == var)
            return "";
        else
            return std::string(var) + "/Library/Logs/";
    }

#ifdef WINDOWS
    inline
    std::string GetLogsLocationOnWin(
        std::function<const wchar_t* (const wchar_t*)> getVar)
    {
        auto var(getVar(L"LOCALAPPDATA"));
        if (nullptr == var)
            return "";
        else
            return Narrow(var) + "\\";
    }

#endif

    inline
    std::string GetLogsLocation()
    {
#ifdef WINDOWS
        return GetLogsLocationOnWin(_wgetenv);
#elif defined(LINUX)
        return GetLogsLocationOnLinux(std::getenv);
#else
        return GetLogsLocationOnMacOSX(std::getenv);
#endif
    }

    inline
    std::string GetUnityLogsDirNameOnLinux()
    {
        return "unity3d/";
    }

    inline
    std::string GetUnityLogsDirNameOnWin()
    {
        return "Unity\\Editor\\";
    }

    inline
    std::string GetUnityLogsDirNameOnMacOSX()
    {
        return "Unity/";
    }

    inline
    std::string GetUnityLogsDirName()
    {
#ifdef WINDOWS
        return GetUnityLogsDirNameOnWin();
#elif LINUX
        return GetUnityLogsDirNameOnLinux();
#else
        return GetUnityLogsDirNameOnMacOSX();
#endif
    }

    inline
    std::string GetUnityLogsLocation()
    {
        return GetLogsLocation() + GetUnityLogsDirName();
    }
}
}
