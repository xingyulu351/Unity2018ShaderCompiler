#pragma once

#include "file_system/File.h"

#include <sstream>

namespace ureport
{
namespace test
{
    class FileFake : public ureport::File
    {
        std::string m_Path;
        std::string m_Content;

    public:
        FileFake(const std::string& path)
            : m_Path(path)
        {
        }

        void SetContent(const std::string& content)
        {
            m_Content = content;
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
            return std::unique_ptr<std::istream>(new std::istringstream(m_Content));
        }

        void Write(std::istream& data)
        {
        }
    };
}
}
