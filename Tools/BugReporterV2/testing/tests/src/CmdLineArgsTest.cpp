#include "CmdLineArgs.h"

#include <UnitTest++.h>

using namespace testing;

SUITE(CmdLineArgs)
{
    TEST(Empty_GivenEmptyCmdLine_ReturnsTrue)
    {
        char* argv[] = { const_cast<char*>("") };
        CmdLineArgs args(0, argv);
        CHECK(args.Empty());
    }

    TEST(Empty_GivenCmdLineWithOneArg_ReturnsTrue)
    {
        char* argv[] = { const_cast<char*>("exe_name") };
        CmdLineArgs args(1, argv);
        CHECK(args.Empty());
    }

    TEST(HasArg_GivenCmdLineWithTheArg_ReturnsTrueWithoutTakingIntoAccountTheCase)
    {
        char* argv[] =
        {
            const_cast<char*>("exe_name"),
            const_cast<char*>("-ArG")
        };
        CmdLineArgs args(2, argv);
        CHECK(args.HasArg("aRg"));
    }

    TEST(HasArg_GivenCmdLineWithTwoArgs_ReturnsTrueForBoth)
    {
        char* argv[] =
        {
            const_cast<char*>("exe_name"),
            const_cast<char*>("-arg1"),
            const_cast<char*>("-arg2")
        };
        CmdLineArgs args(3, argv);
        CHECK(args.HasArg("arg1"));
        CHECK(args.HasArg("arg2"));
    }

    TEST(HasArg_GivenCmdLineWithoutTheArg_ReturnsFalse)
    {
        char* argv[] =
        {
            const_cast<char*>("exe_name"),
            const_cast<char*>("-another_arg")
        };
        CmdLineArgs args(2, argv);
        CHECK(!args.HasArg("arg"));
    }

    TEST(GetArgValue_GivenTheArgValue_ReturnsTheValueWithoutTakingIntoAccountTheCase)
    {
        char* argv[] =
        {
            const_cast<char*>("exe_name"),
            const_cast<char*>("-aRg"),
            const_cast<char*>("arG_vAl")
        };
        CmdLineArgs args(3, argv);
        CHECK_EQUAL("arg_val", args.GetArgValue("arg")[0]);
    }

    TEST(GetArgValue_GivenCmdLineWithoutTheArgValue_ReturnsEmptyVector)
    {
        char* argv[] =
        {
            const_cast<char*>("exe_name"),
            const_cast<char*>("-another_arg"),
            const_cast<char*>("arg_val")
        };
        CmdLineArgs args(3, argv);
        CHECK(args.GetArgValue("arg").empty());
    }

    TEST(GetArgValue_GivenTwoArgsWithValues_ReturnsBoth)
    {
        char* argv[] =
        {
            const_cast<char*>("exe_name"),
            const_cast<char*>("-arg1"),
            const_cast<char*>("arg1_val"),
            const_cast<char*>("-arg2"),
            const_cast<char*>("arg2_val")
        };
        CmdLineArgs args(5, argv);
        CHECK_EQUAL("arg1_val", args.GetArgValue("arg1")[0]);
        CHECK_EQUAL("arg2_val", args.GetArgValue("arg2")[0]);
    }

    TEST(GetArgValue_GivenArgWithTwoValues_ReturnsBoth)
    {
        char* argv[] =
        {
            const_cast<char*>("exe_name"),
            const_cast<char*>("-arg"),
            const_cast<char*>("arg_val1"),
            const_cast<char*>("arg_val2")
        };
        CmdLineArgs args(4, argv);
        CHECK_EQUAL(2, args.GetArgValue("arg").size());
        CHECK_EQUAL("arg_val1", args.GetArgValue("arg")[0]);
        CHECK_EQUAL("arg_val2", args.GetArgValue("arg")[1]);
    }
}
