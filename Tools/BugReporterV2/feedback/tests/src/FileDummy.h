#pragma once

#include "file_system/File.h"

namespace ureport
{
namespace test
{
    class FileDummy : public ureport::File
    {
        std::string GetPath() const
        {
            return std::string();
        }

        bool IsDir() const
        {
            return false;
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
