#pragma once

#include "unity_version/UnityVersion.h"
#include <string>
#include <functional>

namespace ureport
{
    // In case of Mac, the application directory point to the directory
    // actually containing the executable, which is inside of an application bundle,
    // e.g "/Applications/Unity/Unity Bug Reporter.app/Contents/MacOS/".
    // So we need to remove unnecessary subdirs (e.g. "/Unity Bug Reporter.app/Contents/MacOS/")
    // from the dir to have a dir that points to the application bundle,
    // e.g. we need to get "/Applications/Unity"
namespace packages_utils
{
    std::string RemoveBundleDirsOnMac(const std::string& dir)
    {
        size_t slash = dir.length() - 1;      // skip the last character to be on a safe side when there is trailing slash
        for (size_t i = 0; i < 3 && std::string::npos != slash; ++i)
        {
            slash = dir.rfind("/", slash - 1);
        }
        return dir.substr(0, slash);
    }

    std::string GetPackagesLocationOnUnix(std::function<const char* (const char*)> getVar)
    {
        auto var(getVar("XDG_DATA_HOME"));
        if (nullptr != var)
        {
            return std::string(var) + '/';
        }
        else
        {
            std::string dir;
            var = getVar("HOME");
            if (nullptr != var)
            {
                dir = std::string(var) + "/.local/share/";
            }
            return dir;
        }
    }

    std::string GetPackagesLocationOnWin(std::function<const char* (const char*)> getVar)
    {
        auto var(getVar("ProgramData"));
        if (nullptr == var)
            return std::string();
        else
            return std::string(var) + '\\';
    }

    std::string GetPackagesInstallDir(const UnityVersion& unityVersion, std::function<const char* (const char*)> getVar)
    {
#ifdef WINDOWS
        auto dir = GetPackagesLocationOnWin(getVar);
#else
        auto dir = GetPackagesLocationOnUnix(getVar);
#endif
        dir += "Unity/";
        dir += unityVersion.GetMajorNumberStr();
        dir += std::string(".");
        dir += unityVersion.GetMinorNumberStr();
        return dir;
    }
}
}
