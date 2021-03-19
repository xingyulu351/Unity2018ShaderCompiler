#pragma once

#include "FileSystemEntry.h"
#include "FileSystem.h"

namespace ureport
{
namespace details
{
    class FileEntry : public FileSystemEntry
    {
    public:
        FileEntry(const std::string& path, std::shared_ptr<FileSystem> filesystem)
            : m_Path(path)
            , m_FileSystem(filesystem)
        {}

        bool IsDir() const override
        {
            return false;
        }

        virtual std::list<std::string> GetEntries(const std::string&) const override
        {
            std::list<std::string> entries;
            entries.push_back(m_Path);
            return entries;
        }

        virtual std::string GetPath() const override
        {
            return m_Path;
        }

        virtual size_t GetSize() const override
        {
            return m_FileSystem->GetSize(m_Path);
        }

    private:
        std::string m_Path;
        std::shared_ptr<FileSystem> m_FileSystem;
    };
}
}
