#include "SystemInfoCollector.h"
#include "reporter/ReportDraft.h"
#include "file_system/FileSystem.h"
#include "file_system/FileGateway.h"
#include "attachment_preview/AttachmentTextPreview.h"
#include "shims/attribute/GetSize.h"
#include "shims/logical/IsNull.h"

#include <UnitTest++.h>

SUITE(SystemInfoCollector)
{
    using namespace ureport;
    using namespace ureport::collectors;

    class ACollector
    {
    public:
        std::shared_ptr<FileSystem> m_FileSystem;
        std::shared_ptr<FileGateway> m_FileGateway;
        std::shared_ptr<AttachmentFactory> m_AttachmentFactory;
        SystemInfoCollector m_Collector;
        std::shared_ptr<ReportDraft> m_Report;

        ACollector()
            : m_FileSystem(nullptr)
            , m_FileGateway(nullptr)
            , m_AttachmentFactory(CreateAttachmentFactory(m_FileSystem, m_FileGateway))
            , m_Collector(m_AttachmentFactory)
            , m_Report(CreateNewReport())
        {
        }
    };

    TEST_FIXTURE(ACollector, Collect_AttachesOneAttachment)
    {
        m_Collector.Collect(*m_Report);
        auto const attachments = m_Report->GetAttachments();
        CHECK_EQUAL(1, GetSize(attachments));
        CHECK(!IsNull(attachments.front()));
    }

    TEST_FIXTURE(ACollector, Collect_AttachesAttachmentNamedSystemInfo)
    {
        m_Collector.Collect(*m_Report);
        auto const attachments = m_Report->GetAttachments();
        auto const attachment = attachments.front();
        auto const preview = attachment->GetTextPreview();
        CHECK_EQUAL("System Info", preview->GetDescription());
    }
}
