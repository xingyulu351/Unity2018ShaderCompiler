#pragma once

#include "system_interplay/SystemEnvironment.h"

#include <string>

namespace ureport
{
namespace test
{
    class SystemEnvironmentDummy : public ureport::SystemEnvironment
    {
        std::string GetTempDirectory() const
        {
            return std::string();
        }
    };
}
}
