#pragma once

#include <memory>
#include <string>
#include <list>

namespace ureport
{
    class FileSystemEntry;

    class FileSystem
    {
    public:
        virtual bool HasPath(const std::string& path) const = 0;

        virtual std::list<std::string> GetDirEntries(
            const std::string& path, const std::string& filter) const = 0;

        virtual bool IsDir(const std::string& path) const = 0;

        virtual size_t GetPathDepth(const std::string& path) const = 0;

        virtual size_t GetSize(const std::string& path) const = 0;

        virtual ~FileSystem() {}
    };

    std::unique_ptr<FileSystem> CreateFileSystem();

    std::shared_ptr<FileSystemEntry> CreateFileSystemEntry(
        std::shared_ptr<FileSystem> fileSystem, const std::string& path);
}
