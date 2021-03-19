#pragma once

#include "sysinfo/HardwareInfoProvider.h"

namespace ureport
{
namespace test
{
    class HardwareInfoProviderDummy : public ureport::HardwareInfoProvider
    {
        std::string GetBasicHardwareType() const
        {
            return std::string();
        }

        std::string GetGraphicsHardwareType() const
        {
            return std::string();
        }
    };
}
}
