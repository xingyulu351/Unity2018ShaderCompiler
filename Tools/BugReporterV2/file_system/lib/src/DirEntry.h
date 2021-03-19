#pragma once

#include "FileSystemEntry.h"
#include "FileSystem.h"

#include <functional>

namespace ureport
{
namespace details
{
    class DirEntry : public FileSystemEntry
    {
    public:
        DirEntry(const std::string& path, std::shared_ptr<FileSystem> filesystem)
            : m_Path(path)
            , m_FileSystem(filesystem)
        {}

        virtual std::list<std::string> GetEntries(const std::string& filter) const override
        {
            return m_FileSystem->GetDirEntries(m_Path, filter);
        }

        virtual std::string GetPath() const override
        {
            return m_Path;
        }

        virtual bool IsDir() const override
        {
            return true;
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
