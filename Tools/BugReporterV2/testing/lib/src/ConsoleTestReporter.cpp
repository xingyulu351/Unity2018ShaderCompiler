#include "ConsoleTestReporter.h"
#include <algorithm>
#include <stdio.h>
#include <cstring>

using namespace UnitTest;

ConsoleTestReporter::ConsoleTestReporter()
    : m_ShowOnlySummary(false)
    , m_TestNameColumnIndex(100)
    , m_ResultColumnIndex(256)
    , m_IsCurrentlyRunningTest(false)
    , m_CurrentTestIsFailure(false)
{
}

void ConsoleTestReporter::ExpectLogMessage(LogType type, const char* logFragment)
{
    if (!IsCurrentlyRunningTest())
        return;

    m_ExpectedLogMessagesForCurrentTest.push_back(LogMessage(type, logFragment));
}

void ConsoleTestReporter::ReportTestStart(TestDetails const& test)
{
    if (!m_ShowOnlySummary)
    {
        char buffer[1024];
        const size_t suiteNameLength = strlen(test.suiteName);

        // Create '[TestSuite] ' string padded out with blanks up to the column
        // for the test name.
        memset(buffer, ' ', sizeof(buffer));
        buffer[0] = '[';
        memcpy(&buffer[1], test.suiteName, suiteNameLength);
        buffer[suiteNameLength + 1] = ']';
        buffer[std::min<size_t>(m_TestNameColumnIndex, sizeof(buffer))] = '\0';

        // Print '[TestSuite]    TestName'.
        printf("%s%s", buffer, test.testName);

        // Print blanks to pad out to result column.
        const size_t numSpacesToPad = m_ResultColumnIndex - m_TestNameColumnIndex - strlen(test.testName);
        memset(buffer, ' ', sizeof(buffer));
        buffer[std::min<size_t>(numSpacesToPad, sizeof(buffer))] = '\0';
        printf("%s", buffer);
    }

    m_CurrentTest = test;
    m_IsCurrentlyRunningTest = true;
    m_CurrentTestIsFailure = false;
}

void ConsoleTestReporter::ReportFailure(TestDetails const& test, char const* failure)
{
    // Memorize failure.
    Failure details;
    details.fileName = test.filename;
    details.lineNumber = test.lineNumber;
    details.text = failure;
    m_CurrentTestFailures.push_back(details);

    MarkCurrentTestAsFailure();
}

void ConsoleTestReporter::ReportMessage(LogType type, std::string message)
{
    // Check whether we have expected this message to come in.
    for (size_t i = 0; i < m_ExpectedLogMessagesForCurrentTest.size(); ++i)
    {
        // Skip if type doesn't match.
        if (m_ExpectedLogMessagesForCurrentTest[i].first != type)
            continue;

        // Check whether the expected fragment is found in the current message.
        if (message.find(m_ExpectedLogMessagesForCurrentTest[i].second) != std::string::npos)
        {
            // Remove it.  We only accept one occurrence.
            m_ExpectedLogMessagesForCurrentTest.erase(m_ExpectedLogMessagesForCurrentTest.begin() + i);

            // Yes, so all ok.
            return;
        }
    }

    // Not an expected message.  Record.
    m_UnexpectedLogMessagesForCurrentTest.push_back(LogMessage(type, message));

    MarkCurrentTestAsFailure();
}

void ConsoleTestReporter::ReportTestProperty(TestDetails const& test, const char* propName, const TestProperty& propValue)
{
    // Do nothing
}

void ConsoleTestReporter::ReportTestFinish(TestDetails const& test, float secondsElapsed)
{
    m_IsCurrentlyRunningTest = false;

    // If we are still expecting messages, fail the test.
    if (!m_ExpectedLogMessagesForCurrentTest.empty())
        MarkCurrentTestAsFailure();

    if (!m_ShowOnlySummary)
    {
        // Print status.
        if (m_CurrentTestIsFailure)
            printf("FAIL!!!!\n");
        else
            printf("PASS (%ims)\n", (int)(secondsElapsed * 1000.f));

        // Print failures.
        for (size_t i = 0; i < m_CurrentTestFailures.size(); ++i)
        {
            const Failure& failure = m_CurrentTestFailures[i];
            printf("\tCHECK FAILURE: %s\n\t\t(%s:%i)\n",
                failure.text.c_str(),
                failure.fileName.c_str(),
                failure.lineNumber);
        }

        // Print unexpected messages.
        for (size_t i = 0; i < m_UnexpectedLogMessagesForCurrentTest.size(); ++i)
            printf("\tUNEXPECTED %s: %s\n",
                LogTypeToString(m_UnexpectedLogMessagesForCurrentTest[i].first),
                m_UnexpectedLogMessagesForCurrentTest[i].second.c_str());

        // Print expected messages that didn't show.
        for (size_t i = 0; i < m_ExpectedLogMessagesForCurrentTest.size(); ++i)
            printf("\tEXPECTED %s: %s\n",
                LogTypeToString(m_ExpectedLogMessagesForCurrentTest[i].first),
                m_ExpectedLogMessagesForCurrentTest[i].second.c_str());
    }

    // Clear state of current test.
    m_CurrentTestFailures.clear();
    m_UnexpectedLogMessagesForCurrentTest.clear();
    m_ExpectedLogMessagesForCurrentTest.clear();
    m_CurrentTest = TestDetails();
}

void ConsoleTestReporter::ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed)
{
    // Print counters (rely on our fail test counter since we also count unexpected messages
    // as failures).
    printf("Ran %i tests with %i failures in %.2f seconds\n", totalTestCount, (int)m_FailedTests.size(), secondsElapsed);

    // Print failures.
    for (unsigned int i = 0; i < m_FailedTests.size(); ++i)
    {
        const TestDetails& test = m_FailedTests[i];
        printf("\tFAILED: %s [%s]\n", test.testName, test.suiteName);
    }
}
