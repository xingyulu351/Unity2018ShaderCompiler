#include "reporter/ComposeReportController.h"
#include "OperationObserverDummy.h"
#include "ReportDraft.h"
#include "FileSystemDummy.h"
#include "file_system/FileSystemStub.h"
#include "attachment/AttachmentFactoryDummy.h"
#include "attachment/AttachmentDummy.h"
#include "shims/logical/IsNull.h"
#include "test/AssertContains.h"
#include "shims/attribute/GetSize.h"

#include <UnitTest++.h>
#include <memory>
#include <stdexcept>

SUITE(ComposeReportController)
{
    using namespace ureport;
    using namespace ureport::test;

    class NewReportController
    {
    public:
        NewReportController()
            : m_Factory(std::make_shared<AttachmentFactoryDummy>())
            , m_Controller(CreateComposeReportController(
                m_Factory,
                std::make_shared<FileSystemDummy>(),
                [] (ReportDraft&, LongTermOperObserver*) {},
                [] (Report&, LongTermOperObserver*) {},
                [] (Report&, LongTermOperObserver*, const std::string&) {}))
            , m_Report(CreateNewReport())
        {
            m_Controller->SetReport(m_Report);
        }

    public:
        std::shared_ptr<AttachmentFactoryDummy> m_Factory;
        std::unique_ptr<ComposeReportController> m_Controller;
        std::shared_ptr<ReportDraft> m_Report;
    };

    TEST_FIXTURE(NewReportController, SetTitle_GivenTitle_ModifiesReportTitle)
    {
        m_Controller->SetTitle("Title");
        CHECK_EQUAL("Title", m_Report->GetTitle());
    }

    TEST_FIXTURE(NewReportController, SetTextualDescription_GivenNotEmptyString_ModifiesReportDescription)
    {
        m_Controller->SetTextualDescription("Description");
        CHECK_EQUAL("Description", m_Report->GetTextualDescription());
    }

    TEST_FIXTURE(NewReportController, AttachFile_GivenNotEmptyPath_AddsAttachmentToReport)
    {
        m_Controller->AttachFile("path to file");
        CHECK_EQUAL(1, m_Report->GetAttachments().size());
    }

    TEST_FIXTURE(NewReportController, RemoveAttachment_GivenAttachment_RemovesAttachmentFromReport)
    {
        auto attachment = std::make_shared<AttachmentDummy>();
        m_Report->Attach(attachment);
        m_Controller->RemoveAttachment(attachment);
        CHECK_EQUAL(0, GetSize(m_Report->GetAttachments()));
    }

    TEST_FIXTURE(NewReportController, SetReporterEmail_GivenEmail_ModifiesReporterEmail)
    {
        m_Controller->SetReporterEmail("user@site.org");
        CHECK_EQUAL("user@site.org", m_Report->GetReporterEmail());
    }

    class ReportControllerWithStubFileSystem
    {
    public:
        ReportControllerWithStubFileSystem()
            : m_FileSystem(std::make_shared<FileSystemStub>())
            , m_Controller(CreateComposeReportController(
                std::make_shared<AttachmentFactoryDummy>(),
                m_FileSystem,
                [] (ReportDraft&, LongTermOperObserver*) {},
                [] (Report&, LongTermOperObserver*) {},
                [] (Report&, LongTermOperObserver*, const std::string&) {}))
            , m_Report(CreateNewReport())
        {
            m_Controller->SetReport(m_Report);
        }

    public:
        std::shared_ptr<FileSystemStub> m_FileSystem;
        std::unique_ptr<ComposeReportController> m_Controller;
        std::shared_ptr<ReportDraft> m_Report;
    };

    TEST_FIXTURE(ReportControllerWithStubFileSystem, AttachDirectory_GivenNotEmptyPath_AddsAttachmentToReport)
    {
        m_FileSystem->m_PathDepth = 1;
        m_Controller->AttachDirectory("/testdir");
        CHECK_EQUAL(1, GetSize(m_Report->GetAttachments()));
    }

    TEST_FIXTURE(ReportControllerWithStubFileSystem, AttachDirectory_GivenEmptyPath_ThrowsException)
    {
        m_FileSystem->m_PathDepth = 0;
        CHECK_THROW(m_Controller->AttachDirectory(""), std::logic_error);
    }

    TEST_FIXTURE(ReportControllerWithStubFileSystem, AttachDirectory_GivenDootDirPath_ThrowsException)
    {
        m_FileSystem->m_PathDepth = 0;
        CHECK_THROW(m_Controller->AttachDirectory("/"), std::logic_error);
    }

    class ReportControllerWithPrepareReportSpy
    {
    public:
        void Init()
        {
            m_Controller = CreateComposeReportController(
                std::make_shared<AttachmentFactoryDummy>(),
                std::make_shared<FileSystemDummy>(),
                [this] (ReportDraft&, LongTermOperObserver*) { ++m_CollectCalls; },
                [] (Report&, LongTermOperObserver*) {},
                [] (Report&, LongTermOperObserver*, const std::string&) {});
            m_Controller->SetReport(CreateNewReport());
            m_Controller->SetTitle("Title");
            m_Controller->SetReporterEmail("email@server.net");
            m_Controller->SetBugTypeID(Report::kBugProblemWithEditor);
            m_Controller->SetBugReproducibility(
                Report::kReproAlways);
            m_CollectCalls = 0;
        }

    public:
        std::unique_ptr<ComposeReportController> m_Controller;
        int m_CollectCalls;
    };

    TEST_FIXTURE(ReportControllerWithPrepareReportSpy,
        PrepareReport_IsCalledOnlyOnce)
    {
        Init();
        OperationObserverDummy observer;
        m_Controller->PrepareReport(&observer);
        m_Controller->PrepareReport(&observer);
        CHECK_EQUAL(1, m_CollectCalls);
    }

    TEST_FIXTURE(
        ReportControllerWithPrepareReportSpy,
        SendReport_GivenNoPreviousCallOfPreviewReport_CallsCollectSystemData)
    {
        Init();
        OperationObserverDummy observer;
        m_Controller->SendReport(&observer);
        CHECK_EQUAL(1, m_CollectCalls);
    }

    TEST_FIXTURE(
        ReportControllerWithPrepareReportSpy,
        SendReport_GivenPreviousCallOfPreviewReport_DoesntCallCollectSystemData)
    {
        Init();
        OperationObserverDummy observer;
        m_Controller->PrepareReport(&observer);
        m_Controller->SendReport(&observer);
        CHECK_EQUAL(1, m_CollectCalls);
    }

    TEST(SendReport_GivenNoTitle_ThrowsException)
    {
        auto controller(CreateComposeReportController(
            std::make_shared<AttachmentFactoryDummy>(),
            std::make_shared<FileSystemDummy>(),
            [] (ReportDraft&, LongTermOperObserver*) {},
            [] (Report&, LongTermOperObserver*) {},
            [] (Report&, LongTermOperObserver*, const std::string&) {}));
        controller->SetReport(CreateNewReport());
        controller->SetTitle("");
        controller->SetReporterEmail("email@server.net");
        controller->SetBugTypeID(Report::kBugProblemWithEditor);
        controller->SetBugReproducibility(Report::kReproAlways);
        CHECK_THROW(controller->SendReport(nullptr), std::logic_error);
    }

    TEST(SendReport_GivenNoReporterEmail_ThrowsException)
    {
        auto controller(CreateComposeReportController(
            std::make_shared<AttachmentFactoryDummy>(),
            std::make_shared<FileSystemDummy>(),
            [] (ReportDraft&, LongTermOperObserver*) {},
            [] (Report&, LongTermOperObserver*) {},
            [] (Report&, LongTermOperObserver*, const std::string&) {}));
        controller->SetReport(CreateNewReport());
        controller->SetTitle("Title");
        controller->SetBugTypeID(Report::kBugProblemWithEditor);
        controller->SetBugReproducibility(Report::kReproAlways);
        CHECK_THROW(controller->SendReport(nullptr), std::logic_error);
    }

    TEST(SendReport_GivenTitleWithSpacesOnly_ThrowsException)
    {
        auto controller(CreateComposeReportController(
            std::make_shared<AttachmentFactoryDummy>(),
            std::make_shared<FileSystemDummy>(),
            [] (ReportDraft&, LongTermOperObserver*) {},
            [] (Report&, LongTermOperObserver*) {},
            [] (Report&, LongTermOperObserver*, const std::string&) {}));
        controller->SetReport(CreateNewReport());
        controller->SetReporterEmail("email@server.net");
        controller->SetBugTypeID(Report::kBugProblemWithEditor);
        controller->SetBugReproducibility(Report::kReproAlways);
        controller->SetTitle(" \f\n\r\t\v");
        CHECK_THROW(controller->SendReport(nullptr), std::logic_error);
    }

    TEST(SendReport_GivenEmailWithSpacesOnly_ThrowsException)
    {
        auto controller(CreateComposeReportController(
            std::make_shared<AttachmentFactoryDummy>(),
            std::make_shared<FileSystemDummy>(),
            [] (ReportDraft&, LongTermOperObserver*) {},
            [] (Report&, LongTermOperObserver*) {},
            [] (Report&, LongTermOperObserver*, const std::string&) {}));
        controller->SetReport(CreateNewReport());
        controller->SetTitle("Title");
        controller->SetBugTypeID(Report::kBugProblemWithEditor);
        controller->SetBugReproducibility(Report::kReproAlways);
        controller->SetReporterEmail(" \f\n\r\t\v");
        CHECK_THROW(controller->SendReport(nullptr), std::logic_error);
    }

    TEST_FIXTURE(
        ReportControllerWithPrepareReportSpy,
        SaveReport_GivenNoPreviousCallOfPreviewReport_CallsCollectSystemData)
    {
        Init();
        OperationObserverDummy observer;
        m_Controller->SaveReport(&observer, "/some/path");
        CHECK_EQUAL(1, m_CollectCalls);
    }

    TEST_FIXTURE(
        ReportControllerWithPrepareReportSpy,
        SaveReport_GivenPreviousCallOfPreviewReport_DoesntCallCollectSystemData)
    {
        Init();
        OperationObserverDummy observer;
        m_Controller->PrepareReport(&observer);
        m_Controller->SaveReport(&observer, "/some/path");
        CHECK_EQUAL(1, m_CollectCalls);
    }

    TEST_FIXTURE(
        ReportControllerWithPrepareReportSpy,
        SaveReport_GivenFilePathIsEmpty_ThrowsException)
    {
        Init();
        CHECK_THROW(m_Controller->SaveReport(nullptr, ""), std::logic_error);
    }

    TEST_FIXTURE(
        ReportControllerWithPrepareReportSpy,
        SaveReport_GivenFilePathIsWhitespaceOnly_ThrowsException)
    {
        Init();
        CHECK_THROW(m_Controller->SaveReport(nullptr, " \f\n\r\t\v"), std::logic_error);
    }
}
