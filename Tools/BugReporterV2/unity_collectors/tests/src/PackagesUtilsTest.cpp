#include "PackagesUtils.h"
#include "UnityVersionFake.h"

#include <UnitTest++.h>
#include <cstring>

using namespace ureport;

SUITE(PackagesUtils)
{
    TEST(RemoveBundleDirsOnMac_Removes_3_Subdirs)
    {
        auto bundleDir = "/Applications/Unity/Unity Bug Reporter.app/Contents/MacOS/";
        auto resDir = packages_utils::RemoveBundleDirsOnMac(bundleDir);
        CHECK_EQUAL("/Applications/Unity", resDir);
    }

    TEST(RemoveBundleDirsOnMac_Ignores_Trailing_Slash)
    {
        auto bundleDir1 = "/Applications/Unity/Unity Bug Reporter.app/Contents/MacOS/";
        auto bundleDir2 = "/Applications/Unity/Unity Bug Reporter.app/Contents/MacOS";
        auto resDir1 = packages_utils::RemoveBundleDirsOnMac(bundleDir1);
        auto resDir2 = packages_utils::RemoveBundleDirsOnMac(bundleDir2);
        CHECK_EQUAL(resDir1, resDir2);
    }

    TEST(GetPackagesInstallDir_Doesnt_Crash_Given_Empty_Vars)
    {
        test::UnityVersionFake version;
        auto actual = packages_utils::GetPackagesInstallDir(version,
            [] (const char* varName) -> const char*
            {
                return nullptr;
            });
        CHECK_EQUAL("Unity/4.5", actual);
    }

#ifdef WINDOWS

    TEST(GetPackagesInstallDir_Returns_Correct_Dir_Given_ProgramData_Env_Var)
    {
        test::UnityVersionFake version;
        auto actual = packages_utils::GetPackagesInstallDir(version,
            [] (const char* varName) -> const char*
            {
                if (0 == std::strcmp(varName, "ProgramData"))
                    return "C:\\ProgramData";
                else
                    return nullptr;
            });
        CHECK_EQUAL("C:\\ProgramData\\Unity/4.5", actual);
    }

#else

    TEST(GetPackagesInstallDir_Returns_Correct_Dir_Given_XDG_DATA_HOME_Env_Var)
    {
        test::UnityVersionFake version;
        auto actual = packages_utils::GetPackagesInstallDir(version,
            [] (const char* varName) -> const char*
            {
                if (0 == std::strcmp(varName, "XDG_DATA_HOME"))
                    return "/Users/user/.local/share";
                else
                    return nullptr;
            });
        CHECK_EQUAL("/Users/user/.local/share/Unity/4.5", actual);
    }

    TEST(GetPackagesInstallDir_Returns_Correct_Dir_Given_HOME_Env_Var)
    {
        test::UnityVersionFake version;
        auto actual = packages_utils::GetPackagesInstallDir(version,
            [] (const char* varName) -> const char*
            {
                if (0 == std::strcmp(varName, "HOME"))
                    return "/Users/user";
                else
                    return nullptr;
            });
        CHECK_EQUAL("/Users/user/.local/share/Unity/4.5", actual);
    }
#endif
}
