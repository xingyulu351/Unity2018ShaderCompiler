#pragma once

#include <memory>

namespace ureport
{
    class SystemEnvironment;
    class Shell;

    std::unique_ptr<SystemEnvironment> CreateSystemEnvironment();

    std::unique_ptr<Shell> CreateShell();
}
