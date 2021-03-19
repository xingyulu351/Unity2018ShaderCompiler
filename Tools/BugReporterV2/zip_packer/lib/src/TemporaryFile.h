#pragma once

#include "file_system/File.h"

#include <memory>

namespace ureport
{
namespace details
{
    class TemporaryFile : public ureport::File
    {
        std::unique_ptr<ureport::File> m_File;

    public:
        TemporaryFile(std::unique_ptr<ureport::File> file)
            : m_File(std::move(file))
        {
        }

        ~TemporaryFile();

        bool IsDir() const override
        {
            return m_File->IsDir();
        }

        std::string GetPath() const
        {
            return m_File->GetPath();
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
