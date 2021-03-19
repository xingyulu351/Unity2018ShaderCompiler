#include "system_interplay/Gate.h"
#include "QtSystemEnvironment.h"
#include "PosixShell.h"

namespace ureport
{
    std::unique_ptr<SystemEnvironment> CreateSystemEnvironment()
    {
        return std::unique_ptr<SystemEnvironment>(new details::QtSystemEnvironment());
    }

    std::unique_ptr<Shell> CreateShell()
    {
        return std::unique_ptr<Shell>(new posix::Shell());
    }
}
