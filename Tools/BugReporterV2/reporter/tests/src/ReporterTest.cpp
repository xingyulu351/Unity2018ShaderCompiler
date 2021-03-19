#include "ReportWorksheetDummy.h"
#include "CanceledReportWorksheet.h"
#include "ReportWorksheetFake.h"
#include "SenderDummy.h"
#include "SenderSpy.h"
#include "SenderReportSpy.h"
#include "CollectorCallSpy.h"
#include "CollectorDummy.h"
#include "CollectorThatThrows.h"
#include "FilesPackerDummy.h"
#include "settings/SettingsStub.h"
#include "reporter/Reporter.h"
#include "ExporterDummy.h"
#include "reporter/NoWorksheetException.h"
#include "reporter/NoSenderException.h"
#include "file_system/File.h"
#include "NoFilesPackerException.h"

#include <UnitTest++.h>
#include <memory>

using namespace ureport;
using namespace ureport::test;

SUITE(Reporter)
{
    class Fixture
    {
    public:
        std::unique_ptr<Reporter> m_Reporter;
        std::shared_ptr<ReportDraft> m_Report;

        Fixture()
            : m_Reporter(CreateNewReporter())
            , m_Report(CreateNewReport())
        {
            m_Reporter->SetFilesPacker(std::make_shared<FilesPackerDummy>());
            m_Reporter->SetExporter(std::make_shared<ExporterDummy>());
        }
    };

    TEST_FIXTURE(Fixture, DoReport_ForNewReporter_ThrowsNoWorksheetException)
    {
        CHECK_THROW(m_Reporter->DoReport(m_Report), NoWorksheetException);
    }

    TEST_FIXTURE(Fixture, DoReport_WhenWorksheetIsSet_DoesNotThrowException)
    {
        m_Reporter->SetWorksheet(std::make_shared<ReportWorksheetDummy>());
        m_Reporter->SetSender(std::make_shared<SenderDummy>());
        m_Reporter->DoReport(m_Report);
    }

    TEST_FIXTURE(Fixture, DoReport_WhenWorksheetCanceled_DoesNotSendTheReport)
    {
        unsigned int sendCounter = 0;
        m_Reporter->SetWorksheet(std::make_shared<CanceledReportWorksheet>());
        m_Reporter->SetSender(std::make_shared<SenderSpy>(sendCounter));
        m_Reporter->DoReport(m_Report);
        CHECK_EQUAL(0, sendCounter);
    }

    TEST_FIXTURE(Fixture, DoReport_WithNoSender_ThrowsNoSenderException)
    {
        m_Reporter->SetWorksheet(std::make_shared<ReportWorksheetFake>());
        CHECK_THROW(m_Reporter->DoReport(m_Report), NoSenderException);
    }

    TEST_FIXTURE(Fixture, DoReport_SendsReport)
    {
        unsigned int sendCounter = 0;
        m_Reporter->SetWorksheet(std::make_shared<ReportWorksheetFake>());
        m_Reporter->SetSender(std::make_shared<SenderSpy>(sendCounter));
        m_Reporter->DoReport(m_Report);
        CHECK_EQUAL(1, sendCounter);
    }

    TEST_FIXTURE(Fixture, DoReport_GivenFilledReport_SaveEmailSetting)
    {
        auto worksheet = std::make_shared<ReportWorksheetFake>();
        m_Reporter->SetWorksheet(worksheet);
        m_Reporter->SetSender(std::make_shared<SenderDummy>());
        std::shared_ptr<Settings> settings = std::make_shared<SettingsStub>();
        m_Reporter->SetSettings(settings);
        worksheet->m_Email = "email";
        m_Reporter->DoReport(m_Report);
        CHECK_EQUAL("email", settings->GetCustomerEmail());
    }

    TEST_FIXTURE(Fixture, DoReport_GivenCanceledReport_DoesntSaveSettings)
    {
        auto worksheet = std::make_shared<ReportWorksheetFake>();
        worksheet->m_Fill = false;
        m_Reporter->SetWorksheet(worksheet);
        m_Reporter->SetSender(std::make_shared<SenderDummy>());
        std::shared_ptr<Settings> settings = std::make_shared<SettingsStub>();
        m_Reporter->SetSettings(settings);
        worksheet->m_Email = "email";
        m_Reporter->DoReport(m_Report);
        CHECK_EQUAL("", settings->GetCustomerEmail());
    }

    TEST_FIXTURE(Fixture, DoReport_GivenCanceledReport_DoesntSavePublicitySetting)
    {
        auto worksheet = std::make_shared<ReportWorksheetFake>();
        worksheet->m_Fill = false;
        m_Reporter->SetWorksheet(worksheet);
        m_Reporter->SetSender(std::make_shared<SenderDummy>());
        std::shared_ptr<Settings> settings = std::make_shared<SettingsStub>();
        m_Reporter->SetSettings(settings);
        worksheet->m_Publicity = "value";
        m_Reporter->DoReport(m_Report);
        CHECK_EQUAL("", settings->GetPublicity());
    }

    class SenderReportSpyFixture : public Fixture
    {
    public:
        std::shared_ptr<ReportWorksheetFake> m_Worksheet;
        std::shared_ptr<SenderReportSpy> m_Sender;

        SenderReportSpyFixture()
            : m_Worksheet(new ReportWorksheetFake())
            , m_Sender(new SenderReportSpy())
        {
            m_Reporter->SetWorksheet(m_Worksheet);
            m_Reporter->SetSender(m_Sender);
            m_Reporter->SetExporter(std::make_shared<ExporterDummy>());
            m_Reporter->SetFilesPacker(std::make_shared<FilesPackerDummy>());
        }
    };

    TEST_FIXTURE(SenderReportSpyFixture, DoReport_SendsTitle)
    {
        m_Worksheet->m_Title = "Title";
        m_Reporter->DoReport(m_Report);
        CHECK_EQUAL("Title", m_Sender->m_MessageEvent.substr(0, 5));
    }

    TEST_FIXTURE(SenderReportSpyFixture, DoReport_SendsDescription)
    {
        m_Worksheet->m_Description = "Description";
        m_Reporter->DoReport(m_Report);
        CHECK_EQUAL("Description", m_Sender->m_MessageEvent);
    }

    TEST_FIXTURE(SenderReportSpyFixture, DoReport_SendsEmail)
    {
        m_Worksheet->m_Email = "user@site.org.ctr";
        m_Reporter->DoReport(m_Report);
        CHECK_EQUAL("user@site.org.ctr", m_Sender->m_MessageEmail);
    }

    TEST_FIXTURE(Fixture, DoReport_AddOneCollector_UsesAddedCollector)
    {
        m_Reporter->SetWorksheet(std::make_shared<ReportWorksheetFake>());
        m_Reporter->SetSender(std::make_shared<SenderDummy>());
        auto collector = std::make_shared<CollectorCallSpy>();
        m_Reporter->AddCollector(collector);
        m_Reporter->DoReport(m_Report);
        CHECK_EQUAL(1, collector->GetCallCount());
    }

    TEST_FIXTURE(Fixture, DoReport_WhenWorksheetCanceled_DoesNotCallCollectors)
    {
        auto collector = std::make_shared<CollectorCallSpy>();
        m_Reporter->AddCollector(collector);
        m_Reporter->SetWorksheet(std::make_shared<CanceledReportWorksheet>());
        m_Reporter->DoReport(m_Report);
        CHECK_EQUAL(0, collector->GetCallCount());
    }

    TEST_FIXTURE(Fixture, DoReport_WhithNoFilesPacker_ThrowsNoFilesPackerException)
    {
        m_Reporter->SetWorksheet(std::make_shared<ReportWorksheetDummy>());
        m_Reporter->SetSender(std::make_shared<SenderDummy>());
        m_Reporter->SetFilesPacker(nullptr);
        CHECK_THROW(m_Reporter->DoReport(m_Report), NoFilesPackerException);
    }

    TEST_FIXTURE(Fixture, DoReport_TwoCollectorsFirstThrowsException_CallsSecondCollector)
    {
        m_Reporter->SetWorksheet(std::make_shared<ReportWorksheetFake>());
        m_Reporter->SetSender(std::make_shared<SenderDummy>());
        m_Reporter->SetFilesPacker(std::make_shared<FilesPackerDummy>());
        m_Reporter->AddCollector(std::make_shared<CollectorThatThrows>());
        auto collector = std::make_shared<CollectorCallSpy>();
        m_Reporter->AddCollector(collector);
        m_Reporter->DoReport(m_Report);
        CHECK_EQUAL(1, collector->GetCallCount());
    }

    TEST_FIXTURE(Fixture, DoReport_calls_report_initializers_in_direct_order)
    {
        std::string calls;
        m_Reporter->SetWorksheet(std::make_shared<ReportWorksheetDummy>());
        m_Reporter->AddReportInitializer(MakeReportModifier(
            [&calls] (ReportDraft&) { calls += "a"; }));
        m_Reporter->AddReportInitializer(MakeReportModifier(
            [&calls] (ReportDraft&) { calls += "b"; }));
        m_Reporter->DoReport(m_Report);
        CHECK_EQUAL("ab", calls);
    }
}
