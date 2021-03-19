#include "SystemInfoProvider.h"
#include "unity_bridge/SystemInfo.h"

#include <string>

namespace ureport
{
namespace details
{
    class WindowsSystemInfoProvider : public ureport::SystemInfoProvider
    {
        std::string GetOSName() const
        {
            return unity_bridge::GetOperatingSystem();
        }

        Version GetOSVersion() const
        {
            return Version();
        }
    };
}

    std::unique_ptr<SystemInfoProvider> CreateSystemInfoProvider()
    {
        return std::unique_ptr<SystemInfoProvider>(new details::WindowsSystemInfoProvider());
    }
}
