#pragma once

#include "ZipFile.h"

namespace ureport
{
namespace test
{
    class ZipFileDummy : public ureport::ZipFile
    {
        std::string GetPath() const
        {
            return std::string();
        }

        bool IsDir() const
        {
            return false;
        }

        void CreateDirectory(const std::string& name)
        {
        }

        void OpenNewEntry(const std::string& name)
        {
        }

        std::unique_ptr<std::istream> Read() const
        {
            return std::unique_ptr<std::istream>();
        }

        void Write(std::istream& data)
        {
        }
    };
}
}
