#pragma once

#include "file_system/File.h"

namespace ureport
{
namespace details
{
    class PackedAttachmentsContainer : public File
    {
    public:
        PackedAttachmentsContainer(
            const std::string& name,
            std::shared_ptr<File> package)
            : m_Name(name + ".zip")
            , m_Package(package)
        {}

    private:
        std::string GetPath() const
        {
            return m_Name;
        }

        bool IsDir() const override
        {
            return m_Package->IsDir();
        }

        std::unique_ptr<std::istream> Read() const
        {
            return m_Package->Read();
        }

        void Write(std::istream& data)
        {}

    private:
        std::string m_Name;
        std::shared_ptr<File> m_Package;
    };
}
}
