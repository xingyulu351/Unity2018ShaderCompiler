#pragma once

#include "SystemProfile.h"

namespace ureport
{
    class Shell;

namespace macosx
{
    class ShellSystemProfile : public SystemProfile
    {
        std::shared_ptr<Shell> m_Shell;

    public:
        ShellSystemProfile(std::shared_ptr<Shell> shell);

        std::string GetData(const std::vector<DataType>& types) const override;
    };
}
}
