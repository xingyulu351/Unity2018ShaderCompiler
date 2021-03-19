#pragma once

#include <memory>
#include <string>

namespace ureport
{
    class File;

    class FileGateway
    {
    public:
        virtual std::unique_ptr<File> GetFile(const std::string& path) const = 0;

        virtual size_t GetSize(const std::string& path) const = 0;

        virtual ~FileGateway() {}
    };
}
