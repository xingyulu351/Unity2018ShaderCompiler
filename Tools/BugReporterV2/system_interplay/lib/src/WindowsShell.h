#pragma once

#include "system_interplay/Shell.h"
#include "common/StringUtils.h"
#include "shims/attribute/GetCString.h"

#include <stdio.h>
#include <sstream>
#include <memory>

namespace ureport
{
namespace windows
{
    class Shell : public ureport::Shell
    {
        struct ClosePipeOperation
        {
            void operator()(FILE* pipe)
            {
                _pclose(pipe);
            }
        };

        typedef std::unique_ptr<FILE, ClosePipeOperation> Pipe;

    public:
        std::string ExecuteCommand(const std::string& command) override
        {
            Pipe pipe(_wpopen(GetCString(Widen(command)) , L"rt"));
            return Read(pipe);
        }

    private:
        std::string Read(const Pipe& pipe) const
        {
            std::stringstream output;
            while (!feof(pipe.get()))
            {
                const int chunkSize = 1000;
                wchar_t chunk[chunkSize];
                if (fgetws(chunk, chunkSize, pipe.get()) != nullptr)
                {
                    output << Narrow(chunk);
                }
            }
            return output.str();
        }
    };
}
}
