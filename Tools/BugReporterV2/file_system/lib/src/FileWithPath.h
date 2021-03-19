#pragma once

#include "File.h"
#include "FileToRead.h"

#include <memory>
#include <functional>
#include <string>

namespace ureport
{
namespace details
{
    class FileWithPath : public ureport::File
    {
        std::unique_ptr<ureport::File> m_File;
        std::function<std::string(const std::string&)> m_ProcessPath;

    public:
        FileWithPath(std::unique_ptr<ureport::File> file,
                     std::function<std::string(const std::string&)> processPath)
            : m_File(std::move(file))
            , m_ProcessPath(processPath)
        {
        }

        std::string GetPath() const
        {
            return m_ProcessPath(m_File->GetPath());
        }

        bool IsDir() const override
        {
            return m_File->IsDir();
        }

        std::unique_ptr<std::istream> Read() const
        {
            return m_File->Read();
        }

        void Write(std::istream& data)
        {
            m_File->Write(data);
        }
    };
}
}
