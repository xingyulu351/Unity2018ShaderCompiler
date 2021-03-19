#include "sysinfo/SystemInfoProvider.h"

#include <UnitTest++.h>

SUITE(SystemInfoProvider)
{
    using namespace ureport;

    struct Provider
    {
        std::shared_ptr<SystemInfoProvider> m_Provider;

        Provider()
            : m_Provider(CreateSystemInfoProvider())
        {
        }
    };

    TEST_FIXTURE(Provider, GetOSName_ReturnsNonEmptyString)
    {
        CHECK(m_Provider->GetOSName() != "");
    }
}
