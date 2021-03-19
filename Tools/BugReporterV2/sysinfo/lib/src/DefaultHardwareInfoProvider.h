#pragma once

#include "HardwareInfoProvider.h"

namespace ureport
{
namespace details
{
    class DefaultHardwareInfoProvider : public ureport::HardwareInfoProvider
    {
    public:
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
