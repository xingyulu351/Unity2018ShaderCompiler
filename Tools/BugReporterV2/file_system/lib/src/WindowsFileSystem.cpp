#pragma once

#include "FileSystem.h"
#include "common/StringAccessShims.h"

#include <memory>

namespace ureport
{
namespace details
{
    class WindowsFileSystem : public ureport::FileSystem
    {
        bool HasPath(const std::string& path) const
        {
            const int chekExistence = 0;
            return _waccess_s(GetCString(path), chekExistence) != ENOENT;
        }
    };
}

    std::unique_ptr<FileSystem> CreateFileSystem()
    {
        return std::unique_ptr<FileSystem>(new details::WindowsFileSystem());
    }
}
