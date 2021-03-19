#include <UnitTest++.h>

#include "shims/logical/IsNull.h"
#include "LocalFeedbackProvider.h"
#include "ReportFake.h"
#include <QJsonArray>

SUITE(LocalFeedbackProvider)
{
    using namespace ureport;
    using namespace test;
    struct Feedbackers
    {
    public:
        Feedbackers()
        {
            m_Collection = std::shared_ptr<FeedbackCollection>(new FeedbackCollection);
        }

        void GetFeedback(const Report& f)
        {
            m_Provider.GiveFeedback(f, std::shared_ptr<FeedbackCollection>(m_Collection));
        }

        std::map<std::string, Feedback> Actual()
        {
            return m_Collection->AllFeedback;
        }

        LocalFeedbackProvider m_Provider;
        std::shared_ptr<FeedbackCollection> m_Collection;
    };

    TEST_FIXTURE(Feedbackers, AllMandatoryInformationProvided_ReturnsSeverityZero)
    {
        FullReportFake reportFake;
        reportFake.m_BugTypeID = Report::kBugProblemWithEditor;
        reportFake.m_BugRepro = Report::kReproAlways;
        reportFake.m_Title = "Unity has a bug";
        reportFake.m_Email = "someone@unity3d.com";
        GetFeedback(reportFake);
        CHECK_EQUAL(0, Actual()["Summary"].severity);
    }

    TEST_FIXTURE(Feedbackers, BugTypeIDNotSet_ReturnsSeverityThree)
    {
        FullReportFake reportFake;
        reportFake.m_BugRepro = Report::kReproAlways;
        reportFake.m_Title = "Unity has a bug";
        reportFake.m_Email = "someone@unity3d.com";
        GetFeedback(reportFake);
        CHECK_EQUAL(3, Actual()["Summary"].severity);
    }

    TEST_FIXTURE(Feedbackers, BugReproNotSet_ReturnsSeverityThree)
    {
        FullReportFake reportFake;
        reportFake.m_BugTypeID = Report::kBugProblemWithEditor;
        reportFake.m_Title = "Unity has a bug";
        reportFake.m_Email = "someone@unity3d.com";
        GetFeedback(reportFake);
        CHECK_EQUAL(3, Actual()["Summary"].severity);
    }

    TEST_FIXTURE(Feedbackers, EmailEmpty_ReturnsSeverityThree)
    {
        FullReportFake reportFake;
        reportFake.m_BugTypeID = Report::kBugProblemWithEditor;
        reportFake.m_BugRepro = Report::kReproAlways;
        reportFake.m_Title = "Unity has a bug";
        GetFeedback(reportFake);
        CHECK_EQUAL(3, Actual()["Summary"].severity);
    }

    TEST_FIXTURE(Feedbackers, EmailInvalid_ReturnsSeverityThree)
    {
        FullReportFake reportFake;
        reportFake.m_BugTypeID = Report::kBugProblemWithEditor;
        reportFake.m_BugRepro = Report::kReproAlways;
        reportFake.m_Title = "Unity has a bug";
        reportFake.m_Email = "an email";
        GetFeedback(reportFake);
        CHECK_EQUAL(3, Actual()["Summary"].severity);
    }

    TEST_FIXTURE(Feedbackers, TitleEmpty_ReturnsSeverityThree)
    {
        FullReportFake reportFake;
        reportFake.m_BugTypeID = Report::kBugProblemWithEditor;
        reportFake.m_BugRepro = Report::kReproAlways;
        reportFake.m_Email = "someone@unity3d.com";
        GetFeedback(reportFake);
        CHECK_EQUAL(3, Actual()["Summary"].severity);
    }
}
