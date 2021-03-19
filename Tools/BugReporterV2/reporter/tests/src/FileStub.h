#pragma once

#include "file_system/File.h"

#include <string>

namespace ureport
{
namespace test
{
    class FileStub : public ureport::File
    {
        std::string m_Path;

    public:
        FileStub(const std::string& path)
            : m_Path(path)
        {
        }

        std::string GetPath() const
        {
            return m_Path;
        }

        bool IsDir() const
        {
            return false;
        }

        std::unique_ptr<std::istream> Read() const
        {
            return nullptr;
        }

        void Write(std::istream& data)
        {
        }
    };
}
}
