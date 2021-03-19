#pragma once

#include "file_system/File.h"
#include "file_system/FileGateway.h"

namespace ureport
{
namespace test
{
    class FileGatewayStub : public FileGateway
    {
    public:
        std::unique_ptr<File> GetFile(const std::string& path) const override
        {
            return nullptr;
        }

        size_t GetSize(const std::string& path) const override
        {
            return 0;
        }
    };
}
}
