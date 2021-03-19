#pragma once

#include "file_system/FileSystem.h"

namespace ureport
{
namespace test
{
    class FileSystemStub : public ureport::FileSystem
    {
    public:
        bool m_HasPath;
        std::list<std::string> m_DirEntries;
        bool m_IsDir;
        size_t m_PathDepth;
        size_t m_Size;

        FileSystemStub()
        {
            m_HasPath = false;
            std::list<std::string> m_DirEntries = std::list<std::string>();
            m_IsDir = false;
            m_PathDepth = 0;
            m_Size = 0;
        }

        bool HasPath(const std::string& path) const
        {
            return m_HasPath;
        }

        std::list<std::string> GetDirEntries(
            const std::string& path,
            const std::string& nameFilter) const
        {
            return m_DirEntries;
        }

        bool IsDir(const std::string& path) const
        {
            return m_IsDir;
        }

        size_t GetPathDepth(const std::string& path) const override
        {
            return m_PathDepth;
        }

        size_t GetSize(const std::string& path) const override
        {
            return m_Size;
        }
    };
}
}
