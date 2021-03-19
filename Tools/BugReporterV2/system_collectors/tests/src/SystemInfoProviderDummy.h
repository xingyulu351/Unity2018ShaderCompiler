#pragma once

#include "sysinfo/SystemInfoProvider.h"

namespace ureport
{
namespace test
{
    class SystemInfoProviderDummy : public ureport::SystemInfoProvider
    {
        std::string GetOSName() const
        {
            return std::string();
        }

        Version GetOSVersion() const
        {
            return Version();
        }
    };
}
}
