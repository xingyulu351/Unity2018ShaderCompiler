#include <UnitTest++.h>

#include "FeedbackCollection.h"

SUITE(FeedbackCollection)
{
    using namespace ureport;
    struct Collection
    {
    public:
        FeedbackCollection m_Collection;
        std::map<std::string, Feedback> m_Input;
    };

    TEST_FIXTURE(Collection, UpdateLocalFeedbackNoOtherFeedbackAddsFeedback)
    {
        Feedback f;
        m_Input["Test"] = f;
        m_Collection.UpdateLocalFeedback(m_Input);
        CHECK_EQUAL(1, m_Collection.AllFeedback.count("Test"));
    }

    TEST_FIXTURE(Collection, UpdateRemoteFeedbackNoOtherFeedbackAddsFeedback)
    {
        Feedback f;
        m_Input["Test"] = f;
        m_Collection.UpdateRemoteFeedback(m_Input);
        CHECK_EQUAL(1, m_Collection.AllFeedback.count("Test"));
    }

    TEST_FIXTURE(Collection, UpdateLocalFeedbackFeedbackIsLowerSeverityRemoteFeedbackUsed)
    {
        Feedback high;
        high.severity = 2;
        Feedback low;
        low.severity = 0;
        m_Input["Test"] = high;
        m_Collection.UpdateRemoteFeedback(m_Input);
        m_Input["Test"] = low;
        m_Collection.UpdateLocalFeedback(m_Input);
        CHECK_EQUAL(2, m_Collection.AllFeedback.at("Test").severity);
    }

    TEST_FIXTURE(Collection, UpdateRemoteFeedbackFeedbackIsLowerSeverityLocalFeedbackUsed)
    {
        Feedback high;
        high.severity = 2;
        Feedback low;
        low.severity = 0;
        m_Input["Test"] = high;
        m_Collection.UpdateLocalFeedback(m_Input);
        m_Input["Test"] = low;
        m_Collection.UpdateRemoteFeedback(m_Input);
        CHECK_EQUAL(2, m_Collection.AllFeedback.at("Test").severity);
    }

    TEST_FIXTURE(Collection, UpdateRemoteFeedbackFeedbackIsEqualSeverityRemoteFeedbackUsed)
    {
        Feedback loc;
        loc.severity = 1;
        loc.contents = "Local";
        Feedback rem;
        rem.severity = 1;
        rem.contents = "Remote";

        m_Input["Test"] = loc;
        m_Collection.UpdateLocalFeedback(m_Input);
        m_Input["Test"] = rem;
        m_Collection.UpdateRemoteFeedback(m_Input);

        CHECK_EQUAL("Remote", m_Collection.AllFeedback.at("Test").contents);
    }

    TEST_FIXTURE(Collection, UpdateLocalFeedbackFeedbackIsEqualSeverityRemoteFeedbackUsed)
    {
        Feedback loc;
        loc.severity = 1;
        loc.contents = "Local";
        Feedback rem;
        rem.severity = 1;
        rem.contents = "Remote";

        m_Input["Test"] = rem;
        m_Collection.UpdateRemoteFeedback(m_Input);
        m_Input["Test"] = loc;
        m_Collection.UpdateLocalFeedback(m_Input);

        CHECK_EQUAL("Remote", m_Collection.AllFeedback.at("Test").contents);
    }

    TEST_FIXTURE(Collection, UpdateRemoteFeedbackSeverityFourSetsReportBlockingToTrue)
    {
        Feedback absoluteDisaster;
        absoluteDisaster.severity = 4;
        m_Input["Test"] = absoluteDisaster;
        m_Collection.UpdateRemoteFeedback(m_Input);

        CHECK(m_Collection.PreventReporting());
    }

    TEST_FIXTURE(Collection, UpdateRemoteFeedbackSeverityFourRemovesAllOtherFeedback)
    {
        Feedback otherRemote;
        Feedback otherLocal;
        Feedback absoluteDisaster;
        absoluteDisaster.severity = 4;
        m_Input["Local"] = otherLocal;
        m_Collection.UpdateLocalFeedback(m_Input);
        m_Input.clear();
        m_Input["Remote"] = otherRemote;
        m_Input["Test"] = absoluteDisaster;
        m_Collection.UpdateRemoteFeedback(m_Input);

        CHECK_EQUAL(1, m_Collection.AllFeedback.size());
        CHECK_EQUAL(4, m_Collection.AllFeedback.at("Test").severity);
    }

    TEST_FIXTURE(Collection, ResetRemoteFeedbackClearsRemoteFeedback)
    {
        Feedback rem;
        m_Input["Test"] = rem;
        m_Collection.UpdateRemoteFeedback(m_Input);

        m_Collection.ResetRemoteFeedback();

        CHECK_EQUAL(0, m_Collection.AllFeedback.size());
    }

    TEST_FIXTURE(Collection, HasCriticalIssuesSeverityThreeReturnsTrue)
    {
        Feedback desc;
        desc.severity = 3;
        m_Input["Something"] = desc;
        m_Collection.UpdateRemoteFeedback(m_Input);

        CHECK(m_Collection.HasCriticalIssue());
    }

    TEST_FIXTURE(Collection, HasCriticalIssuesNoFeedbackReturnsFalse)
    {
        CHECK(!m_Collection.HasCriticalIssue());
    }
}
