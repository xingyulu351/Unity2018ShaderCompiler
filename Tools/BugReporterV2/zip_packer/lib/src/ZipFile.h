#pragma once

#include "file_system/File.h"

#include <string>

namespace ureport
{
    class ZipFile : public File
    {
    public:
        virtual void CreateDirectory(const std::string& name) = 0;

        virtual void OpenNewEntry(const std::string& name) = 0;

        virtual ~ZipFile() {}
    };
}
