#include "SystemInfoProvider.h"
#include "system_interplay/Shell.h"
#include "system_interplay/Gate.h"

namespace ureport
{
namespace details
{
    class MacXSystemInfoProvider : public ureport::SystemInfoProvider
    {
        std::shared_ptr<Shell> m_Shell;

    public:
        MacXSystemInfoProvider(std::shared_ptr<Shell> shell)
            : m_Shell(shell)
        {
        }

        std::string GetOSName() const
        {
            return m_Shell->ExecuteCommand(
                "sw_vers | perl -pe 's/^[^:]+:\\s*(.*)/$1/;s/\\s+$/ /'; uname -sr;");
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
            new details::MacXSystemInfoProvider(CreateShell()));
    }
}
