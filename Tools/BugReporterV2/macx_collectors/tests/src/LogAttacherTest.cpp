#include "AttachmentFactorySpy.h"
#include "LogAttacher.h"
#include "SystemProfile.h"
#include "reporter/ReportDraft.h"
#include "shims/attribute/GetSize.h"
#include "shims/logical/IsEmpty.h"

#include <UnitTest++.h>

SUITE(LogAttacher)
{
    using namespace ureport;
    using namespace ureport::collectors;
    using namespace ureport::test;
    using namespace ureport::macosx;

    class Attacher
    {
    public:
        std::shared_ptr<AttachmentFactorySpy> m_Factory;
        std::shared_ptr<ReportDraft> m_Report;
        LogAttacher m_Collector;

        Attacher()
            : m_Factory(std::make_shared<AttachmentFactorySpy>())
            , m_Report(CreateNewReport())
            , m_Collector(m_Factory, std::vector<std::string>({"System Profile"}))
        {
        }
    };

    TEST_FIXTURE(Attacher, Collect_AttachesSystemProfileDataAsText)
    {
        m_Report->WriteFact("System Profile", "system profile data");
        m_Collector.Collect(*m_Report);
        auto const attachments = m_Report->GetAttachments();
        CHECK_EQUAL(1, GetSize(attachments));
        CHECK_EQUAL("System Profile", m_Factory->m_Texts.at(0).second);
        CHECK_EQUAL("system profile data", m_Factory->m_Texts.at(0).first);
    }

    TEST_FIXTURE(Attacher, Collect_AttachesNothing_WhenSystemProfileGivesNoData)
    {
        m_Collector.Collect(*m_Report);
        CHECK(IsEmpty(m_Report->GetAttachments()));
    }
}
