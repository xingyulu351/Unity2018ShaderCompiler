#include "FeedbackServiceCommunicator.h"
#include "settings/SettingsStub.h"
#include "settings/Settings.h"

#include <UnitTest++.h>
#include <memory>
#include <QApplication>


SUITE(FeedbackServiceCommunicatorTests) {
    using namespace ureport;
    using namespace ureport::test;

    struct Fixture
    {
    public:
        Fixture()
        {
            int argc = 0;
            char** argv = nullptr;
            m_App = std::unique_ptr<QApplication>(new QApplication(argc, argv));
        }

        std::unique_ptr<QApplication> m_App;
    };

    TEST_FIXTURE(Fixture, GetServerUrlReturnsAddressFromSettings)
    {
        std::shared_ptr<Settings> settings = std::make_shared<SettingsStub>();
        FeedbackServiceCommunicator target(settings);

        CHECK_EQUAL("https://localhost/api/v1.0/bugreporter/feedback", target.GetFeedbackServerUrl());
    }
}
