#pragma once

#include <string>
#include <memory>
#include <vector>

namespace ureport
{
    class HardwareInfoProvider
    {
    public:
        virtual std::string GetBasicHardwareType() const = 0;

        virtual std::string GetGraphicsHardwareType() const = 0;

        virtual ~HardwareInfoProvider() {}
    };

    std::unique_ptr<HardwareInfoProvider> CreateHardwareInfoProvider();
}
