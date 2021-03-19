#include "AttachmentFactorySpy.h"
#include "SystemProfileCollector.h"
#include "SystemProfile.h"
#include "reporter/ReportDraft.h"
#include "shims/attribute/GetSize.h"
#include "shims/logical/IsEmpty.h"

#include <UnitTest++.h>

SUITE(SystemProfileCollector)
{
    using namespace ureport;
    using namespace ureport::collectors;
    using namespace ureport::test;
    using namespace ureport::macosx;

    class SystemProfileStub : public SystemProfile
    {
    public:
        std::string m_Data;

        std::string GetData(const std::vector<DataType>& types) const override
        {
            return m_Data;
        }
    };

    class ACollector
    {
    public:
        std::shared_ptr<SystemProfileStub> m_Profile;
        std::shared_ptr<ReportDraft> m_Report;
        SystemProfileCollector m_Collector;

        ACollector()
            : m_Profile(std::make_shared<SystemProfileStub>())
            , m_Report(CreateNewReport())
            , m_Collector(m_Profile, "System Profile")
        {
        }
    };

    TEST_FIXTURE(ACollector, Collect_WritesFactToReport)
    {
        m_Profile->m_Data = "system profile data";
        m_Collector.Collect(*m_Report);
        CHECK_EQUAL("system profile data", m_Report->ReadFact("System Profile"));
    }
}
