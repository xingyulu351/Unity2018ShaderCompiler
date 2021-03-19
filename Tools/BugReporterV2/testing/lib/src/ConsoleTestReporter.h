#pragma once

#include <TestReporter.h>
#include <TestDetails.h>

#include <string>
#include <vector>

/// The type of the log message in the delegate registered with Application.RegisterLogCallback.
///
enum LogType
{
    /// LogType used for Errors.
    LogType_Error = 0,
    /// LogType used for Asserts. (These indicate an error inside Unity itself.)
    LogType_Assert = 1,
    /// LogType used for Warnings.
    LogType_Warning = 2,
    /// LogType used for regular log messages.
    LogType_Log = 3,
    /// LogType used for Exceptions.
    LogType_Exception = 4,
    /// LogType used for Debug.
    LogType_Debug = 5,
    ///
    LogType_NumLevels
};

inline const char* LogTypeToString(LogType type)
{
    switch (type)
    {
        case LogType_Assert:    return "Assert";
        case LogType_Debug:     return "Debug";
        case LogType_Exception: return "Exception";
        case LogType_Error:     return "Error";
        case LogType_Log:       return "Log";
        case LogType_Warning:   return "Warning";
        default:                return "";
    }
}

/// Unit test reporter that logs to console output.
class ConsoleTestReporter : public UnitTest::TestReporter
{
public:

    ConsoleTestReporter();
    virtual ~ConsoleTestReporter() {}

    void ReportTestStart(UnitTest::TestDetails const& test) override;
    void ReportFailure(UnitTest::TestDetails const& test, char const* failure) override;
    void ReportTestProperty(UnitTest::TestDetails const& test, const char* propName, const UnitTest::TestProperty& propValue) override;
    void ReportTestFinish(UnitTest::TestDetails const& test, float secondsElapsed) override;
    void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed) override;
    void ReportMessage(LogType type, std::string message);

    void ExpectLogMessage(LogType type, const char* logFragment);

    void SetShowOnlySummary(bool value) { m_ShowOnlySummary = value; }
    void SetTestNameColumnIndex(int value) { m_TestNameColumnIndex = value; }
    void SetResultColumnIndex(int value) { m_ResultColumnIndex = value; }

    bool IsCurrentlyRunningTest() const { return m_IsCurrentlyRunningTest; }

private:

    bool m_IsCurrentlyRunningTest;
    bool m_ShowOnlySummary;
    int m_TestNameColumnIndex;
    int m_ResultColumnIndex;

    struct Failure
    {
        std::string text;
        std::string fileName;
        int lineNumber;
    };

    typedef std::pair<LogType, std::string> LogMessage;

    std::vector<UnitTest::TestDetails> m_FailedTests;

    bool m_CurrentTestIsFailure;
    UnitTest::TestDetails m_CurrentTest;
    std::vector<Failure> m_CurrentTestFailures;
    std::vector<LogMessage> m_UnexpectedLogMessagesForCurrentTest;
    std::vector<LogMessage> m_ExpectedLogMessagesForCurrentTest;

    void MarkCurrentTestAsFailure()
    {
        if (m_CurrentTestIsFailure)
            return;

        m_CurrentTestIsFailure = true;
        m_FailedTests.push_back(m_CurrentTest);
    }
};
