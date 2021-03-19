#include "DefaultHardwareInfoProvider.h"
#include "system_interplay/Shell.h"
#include "system_interplay/Gate.h"

namespace ureport
{
namespace details
{
    class MacXHardwareInfoProvider : public DefaultHardwareInfoProvider
    {
        std::shared_ptr<Shell> m_Shell;

    public:
        MacXHardwareInfoProvider(std::shared_ptr<Shell> shell)
            : m_Shell(shell)
        {
        }

        std::string GetBasicHardwareType() const
        {
            return m_Shell->ExecuteCommand("system_profiler SPHardwareDataType | "
                "perl -p -e 's/^\\s+|\\s+$//g;"
                "if (/(?:Machine Model|Model Name|Processor Speed|Processor Name|CPU Speed|Memory): (.*)/)"
                "{ $_ = $1 . \" \" }"
                "elsif (/(?:Total Number Of Cores|Number Of CPUs): (.*)/ && $1 > 1)"
                "{ $_ = $1 . \" CPU cores \" }"
                "else { $_ = \"\" }'");
        }

        std::string GetGraphicsHardwareType() const
        {
            return m_Shell->ExecuteCommand("system_profiler SPDisplaysDataType | "
                "perl -pe 's/^\\s+|\\s+$//g;"
                "if (/(?:Chipset Model): (.*)/)"
                "{ $_ = $1 . \" \" }"
                "elsif (/(?:VRAM .*): (.*)/)"
                "{ $_ = $1 . \" \" }"
                "else { $_ = \"\" }'");
        }
    };
}

    std::unique_ptr<HardwareInfoProvider> CreateHardwareInfoProvider()
    {
        return std::unique_ptr<HardwareInfoProvider>(
            new details::MacXHardwareInfoProvider(CreateShell()));
    }
}
