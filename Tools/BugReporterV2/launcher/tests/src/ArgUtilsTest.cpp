#include "../src/ArgUtils.h"
#include "shims/logical/IsEmpty.h"
#include "shims/attribute/GetSize.h"

#include <UnitTest++.h>
#include <vector>

SUITE(ArgUtils)
{
    using namespace ureport;
    using namespace ureport::launcher;

    class EmptyArgs
    {
    public:
        std::vector<std::string> m_Args;
    };

    TEST_FIXTURE(EmptyArgs, GetFacts_GivenEmptyArgs_ReturnsEmptySet)
    {
        const auto facts = GetFacts(m_Args);
        CHECK(IsEmpty(facts));
    }

    TEST_FIXTURE(EmptyArgs, GetFacts_GivenFactSwitchAndNameOfFact_ReturnsSetWithOneFact)
    {
        m_Args.push_back("--fact");
        m_Args.push_back("Fact");
        const auto facts = GetFacts(m_Args);
        CHECK_EQUAL(1, GetSize(facts));
    }

    TEST_FIXTURE(EmptyArgs, GetFacts_GivenFactSwitchNameAndValue_ReturnsMapWithOneElementWhichHasGivenNameAndValue)
    {
        m_Args.push_back("--fact");
        m_Args.push_back("name");
        m_Args.push_back("value");
        const auto facts = GetFacts(m_Args);
        CHECK_EQUAL(1, GetSize(facts));
        CHECK_EQUAL("value", facts.at("name"));
    }

    TEST(MakeArgsVector_GivenEmptyArgs_ReturnsEmptyVector)
    {
        const auto args = MakeArgsVector(0, nullptr);
        CHECK(IsEmpty(args));
    }

    TEST(MakeArgsVector_GivenThreeArgument_ReturnsVectorWithTheArgumentsInOrder)
    {
        const char* argv[3];
        argv[0] = "first";
        argv[1] = "second";
        argv[2] = "third";
        const std::vector<std::string> args = MakeArgsVector(3, argv);
        CHECK_EQUAL(3, GetSize(args));
        CHECK_EQUAL("first", args[0]);
        CHECK_EQUAL("second", args[1]);
        CHECK_EQUAL("third", args[2]);
    }

    TEST_FIXTURE(EmptyArgs, GetUnityProject_GivenEmptyArgs_ReturnsEmptyString)
    {
        CHECK_EQUAL("", GetUnityProject(m_Args));
    }

    TEST_FIXTURE(EmptyArgs, GetUnityProject_GivenUnityProjectSwitchAndPath_ReturnsPath)
    {
        m_Args.push_back("--unity_project");
        m_Args.push_back("path");
        CHECK_EQUAL("path", GetUnityProject(m_Args));
    }

    TEST_FIXTURE(EmptyArgs, GetUnityProject_GivenWordUnityProjectSwitchAndPath_ReturnsPath)
    {
        m_Args.push_back("word");
        m_Args.push_back("--unity_project");
        m_Args.push_back("path");
        CHECK_EQUAL("path", GetUnityProject(m_Args));
    }

    TEST_FIXTURE(EmptyArgs, GetUnityProject_GivenOnlyUnityProjectSwitch_ReturnsEmtptyString)
    {
        m_Args.push_back("--unity_project");
        CHECK_EQUAL("", GetUnityProject(m_Args));
    }

    TEST_FIXTURE(EmptyArgs, GetUnityProject_GivenTwoUnityProjectsWithPaths_RetunrsFirstPath)
    {
        m_Args.push_back("--unity_project");
        m_Args.push_back("first path");
        m_Args.push_back("--unity_project");
        m_Args.push_back("second path");
        CHECK_EQUAL("first path", GetUnityProject(m_Args));
    }
}
