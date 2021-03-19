#include <UnitTest++.h>

#include "ReportDraft.h"
#include "reporter/ReportChangedMonitor.h"

SUITE(ReportChangedMonitor)
{
    using namespace ureport;

    class ReportChangedMonitorTester
    {
    public:
        std::shared_ptr<ReportDraft> m_Report;
        std::shared_ptr<ReportChangedMonitor> m_Monitor;

        ReportChangedMonitorTester()
            : m_Report(CreateNewReport())
            , m_Monitor(std::make_shared<ReportChangedMonitor>())
        {
            m_Report->AddMonitor(m_Monitor);
        }
    };

    TEST_FIXTURE(ReportChangedMonitorTester, IsReportChanged_GivenNoChangesToReport_ReturnsFalse)
    {
        CHECK(m_Monitor->IsReportChanged() == false);
    }

    TEST_FIXTURE(ReportChangedMonitorTester, IsReportChanged_GivenAnyChangeToReport_ReturnsTrue)
    {
        m_Report->SetTitle("Title");
        CHECK(m_Monitor->IsReportChanged() == true);
    }

    TEST_FIXTURE(ReportChangedMonitorTester, IsReportChanged_WhenBlocked_GivenAnyChangeToReport_ReturnsFalse)
    {
        m_Monitor->Block();
        m_Report->SetTitle("Title");
        CHECK(m_Monitor->IsReportChanged() == false);
    }

    TEST_FIXTURE(ReportChangedMonitorTester, IsReportChanged_WhenUnBlockedAfterBeingBlocked_GivenAnyChangeToReport_ReturnsTrue)
    {
        m_Monitor->Block();
        m_Monitor->UnBlock();
        m_Report->SetTitle("Title");
        CHECK(m_Monitor->IsReportChanged() == true);
    }

    TEST_FIXTURE(ReportChangedMonitorTester, IsReportChanged_AfterResetting_ReturnsFalse)
    {
        m_Report->SetTitle("Title");
        m_Monitor->Reset();
        CHECK(m_Monitor->IsReportChanged() == false);
    }

    TEST_FIXTURE(ReportChangedMonitorTester, IsReportChanged_AfterBeingReset_GivenAnyChange_ReturnsTrue)
    {
        m_Report->SetTitle("Title");
        m_Monitor->Reset();
        m_Report->SetTitle("Title");
        CHECK(m_Monitor->IsReportChanged() == true);
    }
}
