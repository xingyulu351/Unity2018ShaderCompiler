#pragma once

#include <string>

namespace ureport
{
    class Shell
    {
    public:
        virtual std::string ExecuteCommand(const std::string& command) = 0;

        virtual ~Shell() {}
    };
}
