#pragma once

#include "FileSystemDummy.h"

namespace ureport
{
namespace test
{
    class FileSystemFake : public FileSystemDummy
    {
        bool HasPath(const std::string& path) const
        {
            return true;
        }

        bool IsDir(const std::string& path) const
        {
            return path.find("directory") != std::string::npos;
        }

        size_t GetPathDepth(const std::string& path) const override
        {
            return 0;
        }

        size_t GetSize(const std::string& path) const override
        {
            return 0;
        }
    };
}
}
