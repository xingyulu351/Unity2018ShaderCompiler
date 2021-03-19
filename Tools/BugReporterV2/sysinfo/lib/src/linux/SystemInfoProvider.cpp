#include "SystemInfoProvider.h"
#include "system_interplay/Shell.h"
#include "system_interplay/Gate.h"

namespace ureport
{
namespace details
{
    class LinuxSystemInfoProvider : public ureport::SystemInfoProvider
    {
        std::shared_ptr<Shell> m_Shell;

    public:
        LinuxSystemInfoProvider(std::shared_ptr<Shell> shell)
            : m_Shell(shell)
        {
        }

        std::string GetOSName() const
        {
            return m_Shell->ExecuteCommand(
                "uname --kernel-name --kernel-release --processor;" \
                "lsb_release --description --short");
        }

        Version GetOSVersion() const
        {
            return Version();
        }
    };
}

    std::unique_ptr<SystemInfoProvider> CreateSystemInfoProvider()
    {
        return std::unique_ptr<SystemInfoProvider>(
            new details::LinuxSystemInfoProvider(CreateShell()));
    }
}
