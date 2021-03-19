#include "ReportDraft.h"
#include "IncorrectFactNameException.h"
#include "file_system/FileDummy.h"
#include "ReportMonitorSpy.h"
#include "attachment/AttachmentDummy.h"
#include "shims/logical/IsNull.h"
#include "shims/attribute/GetSize.h"
#include "test/AssertContains.h"
#include "attachment/AttachmentDummy.h"

#include <UnitTest++.h>
#include <algorithm>

SUITE(ReportDraft)
{
    using namespace ureport;
    using namespace ureport::test;

    class NewReport
    {
    public:
        std::unique_ptr<ReportDraft> m_Report;

        NewReport()
            : m_Report(CreateNewReport())
        {
        }
    };

    TEST(CanCreateNewReport)
    {
        auto report = CreateNewReport();
        CHECK(!IsNull(report));
    }

    TEST_FIXTURE(NewReport, SetTitle_ChangesTheTitle)
    {
        m_Report->SetTitle("Title");
        CHECK(m_Report->GetTitle() == "Title");
    }

    TEST_FIXTURE(NewReport, NewReportHasEmptyDefaultTitle)
    {
        CHECK(m_Report->GetTitle() == "");
    }

    TEST_FIXTURE(NewReport, NewReportHasEmptyDefaultTextualDescription)
    {
        CHECK(m_Report->GetTextualDescription() == "");
    }

    TEST_FIXTURE(NewReport, NewReportHasBugUnknownByDefault)
    {
        CHECK(m_Report->GetBugTypeID() == Report::kBugUnknown);
    }

    TEST_FIXTURE(NewReport, NewReportHasReproUnknownByDefault)
    {
        CHECK(m_Report->GetBugReproducibility() == Report::kReproUnknown);
    }

    TEST_FIXTURE(NewReport, SetTextualDescription_ChangesTheDescription)
    {
        m_Report->SetTextualDescription("Description");
        CHECK(m_Report->GetTextualDescription() == "Description");
    }

    TEST_FIXTURE(NewReport, ReadFact_GivenEmptyName_ThrowsIncorrectFactNameException)
    {
        CHECK_THROW(m_Report->ReadFact(""), IncorrectFactNameException);
    }

    TEST_FIXTURE(NewReport, ReadFact_GivenUnknownFactName_ReturnsEmptyFact)
    {
        CHECK(m_Report->ReadFact("Unknown fact").empty());
    }

    TEST_FIXTURE(NewReport, WriteFact_NewlyCreatedReport_DoesNotThrowException)
    {
        m_Report->WriteFact("Name", "Fact");
    }

    TEST_FIXTURE(NewReport, WriteFact_NewlyCreatedReport_WritesTheFactToTheReport)
    {
        m_Report->WriteFact("FactName", "TheFact");
        CHECK(m_Report->ReadFact("FactName") == "TheFact");
    }

    TEST_FIXTURE(NewReport, WriteFact_GivenKnownFact_RewritesTheFact)
    {
        m_Report->WriteFact("FactName", "Fact");
        m_Report->WriteFact("FactName", "New fact");
        CHECK(m_Report->ReadFact("FactName") == "New fact");
    }

    TEST_FIXTURE(NewReport, Attach_GivenValidAttachment_AddsTheAttachmentToTheReport)
    {
        auto att(std::make_shared<AttachmentDummy>());
        m_Report->Attach(att);
        AssertContains(m_Report->GetAttachments(), att);
    }

    TEST_FIXTURE(NewReport, Attach_GivenNotNullAttachment_AddsTheAttachmentToTheReport)
    {
        auto attachment = std::make_shared<AttachmentDummy>();
        m_Report->Attach(attachment);
        AssertContains(m_Report->GetAttachments(), attachment);
    }

    class NewMonitoredReport : public NewReport
    {
    public:
        std::shared_ptr<ReportMonitorSpy> m_Monitor;

        NewMonitoredReport()
            : m_Monitor(std::make_shared<ReportMonitorSpy>())
        {
            m_Report->AddMonitor(m_Monitor);
        }
    };

    TEST_FIXTURE(NewMonitoredReport, SetTitle_GivenTitle_NotifiesReportMonitor)
    {
        m_Report->SetTitle("Title");
        CHECK_EQUAL(1, m_Monitor->GetUpdateCount());
    }

    TEST_FIXTURE(NewMonitoredReport, SetTextualDescription_GivenNonEmptyString_NotifiesReportMonitor)
    {
        m_Report->SetTextualDescription("Description");
        CHECK_EQUAL(1, m_Monitor->GetUpdateCount());
    }

    TEST_FIXTURE(NewMonitoredReport, SetReporterEmail_GivenEmail_NotifiesReportMonitor)
    {
        m_Report->SetReporterEmail("e-mail");
        CHECK_EQUAL(1, m_Monitor->GetUpdateCount());
    }

    TEST_FIXTURE(NewMonitoredReport, WriteFact_GivenFact_NotifiesReportMonitor)
    {
        m_Report->WriteFact("name", "fact");
        CHECK_EQUAL(1, m_Monitor->GetUpdateCount());
    }

    TEST_FIXTURE(NewMonitoredReport, Attach_GivenNotNullAttachment_NotifiesReportMonitor)
    {
        m_Report->Attach(std::make_shared<AttachmentDummy>());
        CHECK_EQUAL(1, m_Monitor->GetUpdateCount());
    }

    TEST_FIXTURE(NewMonitoredReport, Detach_GivenAttachmentFromTheReport_RemovesTheAttachment)
    {
        auto attachment = std::make_shared<AttachmentDummy>();
        m_Report->Attach(attachment);
        m_Report->Detach(attachment);
        CHECK_EQUAL(0, GetSize(m_Report->GetAttachments()));
    }

    TEST_FIXTURE(NewMonitoredReport, Detach_GivenAttachmentNotFromTheReport_DoesNotRemoveAnyAttachmentFromTheReport)
    {
        auto attached = std::make_shared<AttachmentDummy>();
        auto free = std::make_shared<AttachmentDummy>();
        m_Report->Attach(attached);
        m_Report->Detach(free);
        CHECK_EQUAL(1, GetSize(m_Report->GetAttachments()));
        AssertContains(m_Report->GetAttachments(), attached);
    }

    TEST_FIXTURE(NewMonitoredReport, Detach_GivenAttachmentFromTheReport_NotifiesReportMonitor)
    {
        auto attachment = std::make_shared<AttachmentDummy>();
        m_Report->Attach(attachment);
        m_Report->Detach(attachment);
        CHECK_EQUAL(2, m_Monitor->GetUpdateCount());
    }
}
