#include "ReportSettingsLoader.h"
#include "settings/SettingsStub.h"
#include "reporter/ReportDraft.h"

#include <UnitTest++.h>

using namespace ureport;
using namespace ureport::test;
using namespace ureport::launcher;

SUITE(ReportSettingsLoaderTests)
{
    class Loader
    {
    public:
        Loader()
            : m_Settings(std::make_shared<SettingsStub>())
            , m_Loader(ReportSettingsLoader(m_Settings))
            , m_Report(CreateNewReport())
        {
        }

    public:
        std::shared_ptr<SettingsStub> m_Settings;
        ReportSettingsLoader m_Loader;
        std::unique_ptr<ReportDraft> m_Report;
    };

    TEST_FIXTURE(Loader, Sets_reporter_email)
    {
        m_Settings->SetCustomerEmail("email");
        m_Loader.Modify(*m_Report);
        CHECK_EQUAL("email", m_Report->GetReporterEmail());
    }
}
