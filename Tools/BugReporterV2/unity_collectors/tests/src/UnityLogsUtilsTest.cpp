#include "UnityLogsUtils.h"

#include <UnitTest++.h>

using namespace ureport;
using namespace unity_logs_utils;

SUITE(UnityLogsUtils)
{
    TEST(GetLogsLocationOnMacOSX_Given_Empty_Env_Vars_Returns_Empty_Location)
    {
        auto loc = GetLogsLocationOnMacOSX([] (const char*) -> const char* { return nullptr; });
        CHECK_EQUAL("", loc);
    }

    TEST(GetUnityLogsLocationOnMacOSX_Given_Env_Var_Returns_Correct_Location)
    {
        auto loc =
            GetLogsLocationOnMacOSX([] (const char*) { return "/Users/user"; }) +
            GetUnityLogsDirNameOnMacOSX();
        CHECK_EQUAL("/Users/user/Library/Logs/Unity/", loc);
    }

#ifdef WINDOWS
    TEST(GetLogsLocationOnWin_Given_Empty_Env_Vars_Returns_Empty_Location)
    {
        auto loc = GetLogsLocationOnWin([] (const wchar_t*) { return nullptr; });
        CHECK_EQUAL("", loc);
    }

    TEST(GetUnityLogsLocationOnWin_Given_Env_Var_Returns_Correct_Location)
    {
        auto loc =
            GetLogsLocationOnWin([] (const wchar_t*) {
                return L"C:\\Users\\user\\AppData\\Local";
            }) +
            GetUnityLogsDirNameOnWin();
        CHECK_EQUAL("C:\\Users\\user\\AppData\\Local\\Unity\\Editor\\", loc);
    }
#endif
}
