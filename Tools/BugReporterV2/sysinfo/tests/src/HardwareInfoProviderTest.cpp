#include "sysinfo/HardwareInfoProvider.h"

#include <UnitTest++.h>

SUITE(HardwareInfoProvider)
{
    using namespace ureport;

    struct Provider
    {
        std::shared_ptr<HardwareInfoProvider> m_Provider;

        Provider()
            : m_Provider(CreateHardwareInfoProvider())
        {
        }
    };

    TEST_FIXTURE(Provider, GetBasicHardwareType_ReturnsNonEmptyString)
    {
        CHECK(m_Provider->GetBasicHardwareType() != "");
    }

#ifndef LINUX
    // We don't do enough gui setup to run this test on Linux
    TEST_FIXTURE(Provider, GetGraphicsHardwareType_ReturnsNonEmptyString)
    {
        CHECK(m_Provider->GetGraphicsHardwareType() != "");
    }

#endif
}
