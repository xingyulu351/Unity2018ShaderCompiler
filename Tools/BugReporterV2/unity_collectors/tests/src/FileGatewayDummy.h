#pragma once

#include "file_system/FileGateway.h"
#include "file_system/File.h"

namespace ureport
{
namespace test
{
    class FileGatewayDummy : public ureport::FileGateway
    {
        std::unique_ptr<File> GetFile(const std::string& path) const
        {
            return nullptr;
        }
    };
}
}
