#include <UnitTest++.h>

#include "ReportDummy.h"
#include "BasicFeedbackManager.h"
#include "FakeFeedbackProvider.h"

SUITE(FeedbackManager)
{
    using namespace ureport;
    using namespace test;
    struct Manager
    {
    public:
        Manager()
        {
            auto a = std::unique_ptr<FeedbackProvider>(new FakeFeedbackProvider());
            auto b = std::unique_ptr<FeedbackProvider>(new FakeFeedbackProvider());
            m_Manager = std::unique_ptr<BasicFeedbackManager>(new BasicFeedbackManager(std::move(a), std::move(b)));
        }

        std::unique_ptr<BasicFeedbackManager> m_Manager;
    };

    TEST_FIXTURE(Manager, FeedbackReceived_CallbackCalled)
    {
        bool callbacked = false;
        m_Manager->GiveFeedback(ReportDummy(), [&](std::map<std::string, Feedback> collection)
        {
            callbacked = true;
        });
        CHECK(callbacked);
    }
}
