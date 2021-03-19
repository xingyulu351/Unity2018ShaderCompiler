#pragma once

#include "Windows.h"
#include "common/StringUtils.h"

#include <string>
#include <vector>
#include <memory>

namespace ureport
{
namespace windows
{
    inline
    std::vector<std::string> MakeArgsVector(LPWSTR commandLine)
    {
        if (commandLine == nullptr)
            return std::vector<std::string>();
        std::vector<std::string> args;
        int argc = 0;
        std::unique_ptr<LPWSTR, HLOCAL(__stdcall *)(HLOCAL)> argv(
            ::CommandLineToArgvW(commandLine, &argc), ::LocalFree);
        for (int i = 0; i < argc; ++i)
        {
            args.push_back(Narrow(argv.get()[i]));
        }
        return args;
    }
}
}
