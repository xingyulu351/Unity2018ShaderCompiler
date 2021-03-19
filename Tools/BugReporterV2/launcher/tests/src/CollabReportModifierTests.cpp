#include "attachment/AttachmentFactoryDummy.h"
#include "CollabReportModifier.h"
#include "file_system/FileSystemStub.h"
#include "reporter/ReportDraft.h"
#include "shims/logical/IsNull.h"
#include "shims/attribute/GetSize.h"

#include <UnitTest++.h>
#include <memory>
#include <string>

SUITE(CollabReportModifier)
{
    using namespace ureport;
    using namespace ureport::test;

    class NewCollabReportModifier
    {
    public:
        NewCollabReportModifier()
            : m_AttachmentFactory(new AttachmentFactoryDummy())
            , m_FileSystem(new FileSystemStub())
        {
            m_ReportModifier = std::make_shared<CollabReportModifier>("projectpath", m_FileSystem, m_AttachmentFactory);
            m_Report = CreateNewReport();
            m_FileSystem->m_HasPath = true;
        }

    public:
        std::shared_ptr<CollabReportModifier> m_ReportModifier;
        std::shared_ptr<AttachmentFactoryDummy> m_AttachmentFactory;
        std::shared_ptr<FileSystemStub> m_FileSystem;
        std::unique_ptr<ReportDraft> m_Report;
        std::string m_ProjectPath;
    };

    TEST_FIXTURE(NewCollabReportModifier, CanCreateCollabReportModifier)
    {
        CHECK(!IsNull(m_ReportModifier));
    }

    TEST_FIXTURE(NewCollabReportModifier, Modify_IfSnapshotFileDoesNotExist_DoesNotAttachAnything)
    {
        m_ReportModifier->Modify(*m_Report);

        CHECK_EQUAL(0, GetSize(m_Report->GetAttachments()));
    }

    TEST_FIXTURE(NewCollabReportModifier, Modify_IfSnapshotFilesExists_AttachesIt)
    {
        m_FileSystem->m_DirEntries.push_back("CollabSnapshot_x.txt");

        m_ReportModifier->Modify(*m_Report);

        CHECK_EQUAL(1, GetSize(m_Report->GetAttachments()));
    }

    TEST_FIXTURE(NewCollabReportModifier, Modify_IfSnapshotMultipleFilesExists_AttachesAll)
    {
        m_FileSystem->m_DirEntries.push_back("CollabSnapshot_x.txt");
        m_FileSystem->m_DirEntries.push_back("CollabSnapshot_y.txt");
        m_FileSystem->m_DirEntries.push_back("CollabSnapshot_z.txt");

        m_ReportModifier->Modify(*m_Report);

        CHECK_EQUAL(3, GetSize(m_Report->GetAttachments()));
    }

    TEST_FIXTURE(NewCollabReportModifier, Modify_AttachesOnlySnapshotFiles)
    {
        m_FileSystem->m_DirEntries.push_back("CollabSnapshot_x.txt");
        m_FileSystem->m_DirEntries.push_back("1.txt");
        m_FileSystem->m_DirEntries.push_back("abc");

        m_ReportModifier->Modify(*m_Report);

        CHECK_EQUAL(1, GetSize(m_Report->GetAttachments()));
    }
}
