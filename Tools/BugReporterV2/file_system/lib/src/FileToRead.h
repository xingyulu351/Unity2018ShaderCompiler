#pragma once

#include "File.h"
#include "system_interplay/FileSystem.h"

#include <fstream>
#include <ios>

namespace ureport
{
namespace details
{
    class FileToRead : public ureport::File
    {
        std::string m_Path;

    public:

        FileToRead(const std::string& path)
            : m_Path(path)
        {
        }

        std::string GetPath() const
        {
            return m_Path;
        }

        bool IsDir() const override
        {
            return CreateFileSystem()->IsDir(GetPath());
        }

        std::unique_ptr<std::istream> Read() const
        {
            return std::unique_ptr<std::istream>(OpenFileStream(GetPath(), std::ios::in | std::ios::binary));
        }

        void Write(std::istream& data)
        {
        }
    };
}
}
