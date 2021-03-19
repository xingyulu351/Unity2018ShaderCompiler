#pragma once

#include "system_interplay/Shell.h"
#include "shims/attribute/GetCString.h"

#include <sstream>
#include <memory>

namespace ureport
{
namespace posix
{
    class Shell : public ureport::Shell
    {
        struct ClosePipeOperation
        {
            void operator()(FILE* pipe)
            {
                pclose(pipe);
            }
        };

        typedef std::unique_ptr<FILE, ClosePipeOperation> Pipe;

    public:
        std::string ExecuteCommand(const std::string& command) override
        {
            Pipe pipe(popen(GetCString(command), "r"));
            return Read(pipe);
        }

    private:
        std::string Read(const Pipe& pipe) const
        {
            std::stringstream output;
            while (!feof(pipe.get()))
            {
                const int chunkSize = 1000;
                char chunk[chunkSize] = {0};
                if (fgets(chunk, chunkSize, pipe.get()) != nullptr)
                {
                    output << chunk;
                }
            }
            return output.str();
        }
    };
}
}
