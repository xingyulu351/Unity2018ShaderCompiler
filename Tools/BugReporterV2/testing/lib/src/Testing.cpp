#include "TestingUtils.h"
#include "CmdLineArgs.h"
#include "ConsoleTestReporter.h"

#include <UnitTest++.h>
#include <NunitTestReporter.h>
#include <TestReporterStdout.h>
#include <TestList.h>
#include <algorithm>
#include <string>
#include <functional>
#include <map>

using namespace UnitTest;

namespace testing
{
    struct LongestNameCalculator
    {
        int longestSuiteNameLength;
        int longestTestNameLength;

        LongestNameCalculator() :
            longestSuiteNameLength(0),
            longestTestNameLength(0)
        {
        }

        void operator()(const UnitTest::Test* test)
        {
            longestSuiteNameLength = std::max<int>((int)strlen(test->m_details.suiteName), longestSuiteNameLength);
            longestTestNameLength = std::max<int>((int)strlen(test->m_details.testName), longestTestNameLength);
        }
    };

    template<typename Predicate>
    static int RunUnitTests(bool summaryOnly, const Predicate& predicate)
    {
        LongestNameCalculator longestNames;
        TestList& allTests = Test::GetTestList();
        allTests.ForEachTest(longestNames);

        ConsoleTestReporter reporter;
        reporter.SetShowOnlySummary(summaryOnly);
        reporter.SetTestNameColumnIndex(longestNames.longestSuiteNameLength + 4);
        reporter.SetResultColumnIndex(longestNames.longestSuiteNameLength + 4 + longestNames.longestTestNameLength + 4);
        UnitTest::TestRunner runner(reporter);
        return runner.RunTestsIf(allTests, NULL, predicate, 0);
    }

    struct TestName
    {
        std::string suiteName, testName;

        TestName(std::string suiteName, std::string testName) :
            suiteName(suiteName),
            testName(testName)
        {
        }
    };

    template<typename Predicate>
    struct TestGatherer
    {
        std::map<std::string, std::vector<TestName> > tests;
        const Predicate& filter;

        TestGatherer(const Predicate& filter) :
            filter(filter)
        {
        }

        void operator()(const UnitTest::Test* test)
        {
            if (!filter(test))
                return;

            std::vector<TestName>& testsForFile = tests[test->m_details.filename];
            testsForFile.push_back(TestName(test->m_details.suiteName, test->m_details.testName));
        }
    };

    template<typename Predicate>
    static void PrintUnitTestList(const Predicate& filter)
    {
        TestGatherer<Predicate> testGatherer(filter);
        Test::GetTestList().ForEachTest(testGatherer);

        int matchingTestCount = 0;
        for (std::map<std::string, std::vector<TestName> >::const_iterator fileIt = testGatherer.tests.begin(); fileIt != testGatherer.tests.end(); fileIt++)
        {
            // Print filename.
            printf("%s:\n", fileIt->first.c_str());

            // List matching tests in file.
            for (std::vector<TestName>::const_iterator testIt = fileIt->second.begin(); testIt != fileIt->second.end(); testIt++)
            {
                printf("\t[%s] %s\n", testIt->suiteName.c_str(), testIt->testName.c_str());
                ++matchingTestCount;
            }
        }

        printf("%i test%s\n", matchingTestCount, matchingTestCount == 1 ? "" : "s");
    }

    struct TestFilter
    {
        std::vector<std::string> m_MatchNames;
        TestFilter(const std::vector<std::string>& matchNames)
            : m_MatchNames(matchNames)
        {
            std::for_each(m_MatchNames.begin(), m_MatchNames.end(),
                std::function<std::string(const std::string&)>(ToLower));
        }

        bool operator()(const Test* test) const
        {
            if (m_MatchNames.empty())
                return true;
            std::string suiteName = ToLower(test->m_details.suiteName);
            std::string testName = ToLower(test->m_details.testName);
            for (std::size_t i = 0; i < m_MatchNames.size(); ++i)
            {
                if (suiteName.find(m_MatchNames[i]) != std::string::npos ||
                    testName.find(m_MatchNames[i]) != std::string::npos)
                    return true;
            }
            return false;
        }
    };

    int RunUnitTestsIfRequired(const CmdLineArgs& args)
    {
        if (args.Empty())
        {
            return UnitTest::RunAllTests();
        }
        else if (args.HasArg("runNativeTests"))
        {
            TestFilter filter(args.GetArgValue("runNativeTests"));
            return RunUnitTests(args.HasArg("nativeTestsSummaryOnly"), filter);
        }
        else if (args.HasArg("listNativeTests"))
        {
            TestFilter filter(args.GetArgValue("listNativeTests"));
            PrintUnitTestList(filter);
        }
        return 0;
    }
}

int main(int argc, char* argv[])
{
    testing::CmdLineArgs args(argc, argv);
    return testing::RunUnitTestsIfRequired(args);
}
