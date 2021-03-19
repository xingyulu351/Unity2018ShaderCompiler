#pragma once

#include "Version.h"

#include <string>
#include <memory>

namespace ureport
{
    class SystemInfoProvider
    {
    public:
        virtual std::string GetOSName() const = 0;

        virtual Version GetOSVersion() const = 0;

        virtual ~SystemInfoProvider() {}
    };

    std::unique_ptr<SystemInfoProvider> CreateSystemInfoProvider();
}
