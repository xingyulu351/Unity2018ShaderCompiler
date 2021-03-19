#include "system_interplay/Gate.h"
#include "WindowsSystemEnvironment.h"
#include "WindowsShell.h"

namespace ureport
{
    std::unique_ptr<SystemEnvironment> CreateSystemEnvironment()
    {
        return std::unique_ptr<SystemEnvironment>(
            new windows::SystemEnvironment());
    }

    std::unique_ptr<Shell> CreateShell()
    {
        return std::unique_ptr<Shell>(new windows::Shell());
    }
}
