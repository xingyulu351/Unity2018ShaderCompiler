#include "attachment/AttachmentFactoryDummy.h"
#include "UnityPackageManagerReportModifier.h"
#include "file_system/FileSystemStub.h"
#include "reporter/ReportDraft.h"
#include "shims/logical/IsNull.h"
#include "shims/attribute/GetSize.h"

#include <UnitTest++.h>
#include <memory>
#include <string>

SUITE(UnityPackageManagerReportModifier)
{
    using namespace ureport;
    using namespace ureport::test;

    class NewUnityPackageManagerReportModifier
    {
    public:
        NewUnityPackageManagerReportModifier()
            : m_AttachmentFactory(new AttachmentFactoryDummy())
            , m_FileSystem(new FileSystemStub())
        {
            m_ReportModifier = std::make_shared<UnityPackageManagerReportModifier>("projectpath", m_FileSystem, m_AttachmentFactory);
            m_Report = CreateNewReport();
            m_FileSystem->m_HasPath = true;
        }

    public:
        std::shared_ptr<UnityPackageManagerReportModifier> m_ReportModifier;
        std::shared_ptr<AttachmentFactoryDummy> m_AttachmentFactory;
        std::shared_ptr<FileSystemStub> m_FileSystem;
        std::unique_ptr<ReportDraft> m_Report;
        std::string m_ProjectPath;
    };

    TEST_FIXTURE(NewUnityPackageManagerReportModifier, CanCreateUnityPackageManagerReportModifier)
    {
        CHECK(!IsNull(m_ReportModifier));
    }

    TEST_FIXTURE(NewUnityPackageManagerReportModifier, Modify_IfManifestFileDoesNotExist_DoesNotAttachAnything)
    {
        m_ReportModifier->Modify(*m_Report);

        CHECK_EQUAL(0, GetSize(m_Report->GetAttachments()));
    }

    TEST_FIXTURE(NewUnityPackageManagerReportModifier, Modify_IfManifestFilesExists_AttachesIt)
    {
        m_FileSystem->m_DirEntries.push_back("manifest.json");

        m_ReportModifier->Modify(*m_Report);

        CHECK_EQUAL(1, GetSize(m_Report->GetAttachments()));
    }

    TEST_FIXTURE(NewUnityPackageManagerReportModifier, Modify_AttachesOnlyManifestFiles)
    {
        m_FileSystem->m_DirEntries.push_back("manifest.json");
        m_FileSystem->m_DirEntries.push_back("1.txt");
        m_FileSystem->m_DirEntries.push_back("abc");

        m_ReportModifier->Modify(*m_Report);

        CHECK_EQUAL(1, GetSize(m_Report->GetAttachments()));
    }
}
