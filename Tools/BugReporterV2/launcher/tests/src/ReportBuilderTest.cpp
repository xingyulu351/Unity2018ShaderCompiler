#include "ReportBuilder.h"
#include "Args.h"
#include "ArgsStub.h"
#include "AttachmentFactorySpy.h"
#include "FileSystemFake.h"
#include "reporter/ReportDraft.h"
#include "attachment/Attachment.h"
#include "attachment/AttachmentFactory.h"
#include "shims/logical/IsNull.h"
#include "shims/attribute/GetSize.h"

#include <UnitTest++.h>

SUITE(ReportBuilder)
{
    using namespace ureport;
    using namespace ureport::test;

    class FreshBuilder
    {
    public:
        std::shared_ptr<AttachmentFactorySpy> m_AttachmentFactory;
        std::shared_ptr<FileSystem> m_FileSystem;
        std::unique_ptr<ReportBuilder> m_Builder;

        FreshBuilder()
            : m_AttachmentFactory(std::make_shared<AttachmentFactorySpy>())
            , m_FileSystem(std::make_shared<FileSystemFake>())
            , m_Builder(CreateReportBuilder(m_AttachmentFactory, m_FileSystem))
        {
        }
    };

    TEST_FIXTURE(FreshBuilder, BuildReport_GivenArgsWithUnityProject_CreatesReportWithTheProjectAttached)
    {
        ArgsStub args;
        args.m_Values["unity_project"].push_back("unity project");
        const auto report = m_Builder->BuildReport(args);
        const auto attachments = report->GetAttachments();
        CHECK_EQUAL(1, GetSize(attachments));
        CHECK_EQUAL("directory:unity project", m_AttachmentFactory->m_Calls[0]);
    }

    TEST_FIXTURE(FreshBuilder, BuildReport_GivenOneFileAndOneDirectoryAttachments_CreatesReportWithTheAttachments)
    {
        ArgsStub args;
        args.m_Values["attach"].push_back("file attachment");
        args.m_Values["attach"].push_back("directory attachment");
        const auto report = m_Builder->BuildReport(args);
        const auto attachments = report->GetAttachments();
        CHECK_EQUAL(2, GetSize(attachments));
        CHECK_EQUAL("file:file attachment", m_AttachmentFactory->m_Calls[0]);
        CHECK_EQUAL("directory:directory attachment", m_AttachmentFactory->m_Calls[1]);
    }

    TEST_FIXTURE(FreshBuilder, BuildReport_GivenEditorMode_CreatesReportWithUnityEditorModeFact)
    {
        ArgsStub args;
        args.m_Values["editor_mode"].push_back("unity editor mode");
        const auto report = m_Builder->BuildReport(args);
        CHECK_EQUAL("unity editor mode", report->ReadFact("UnityEditorMode"));
    }

    TEST(BuildReport_GivenNotExistingFile_CreatesReportWithNoFileAttached)
    {
        ArgsStub args;
        args.m_Values["attach"].push_back("not existing file");
        const auto builder = CreateReportBuilder(std::make_shared<AttachmentFactoryDummy>(),
            std::make_shared<FileSystemDummy>());
        const auto report = builder->BuildReport(args);
        CHECK_EQUAL(0, GetSize(report->GetAttachments()));
    }

    TEST(BuildReport_GivenNotExistingUnityProject_CreatesReportWithNoAttachment)
    {
        ArgsStub args;
        args.m_Values["unity_project"].push_back("not existing project");
        const auto builder = CreateReportBuilder(std::make_shared<AttachmentFactoryDummy>(),
            std::make_shared<FileSystemDummy>());
        const auto report = builder->BuildReport(args);
        CHECK_EQUAL(0, GetSize(report->GetAttachments()));
    }
}
