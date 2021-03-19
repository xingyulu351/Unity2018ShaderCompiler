#pragma once

#include <string>

namespace ureport
{
    class SystemEnvironment
    {
    public:
        virtual std::string GetTempDirectory() const = 0;

        virtual ~SystemEnvironment() {}
    };
}
