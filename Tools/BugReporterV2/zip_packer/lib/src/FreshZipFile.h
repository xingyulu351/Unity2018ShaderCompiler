#pragma once

#include "BasicZipFile.h"
#include "shims/attribute/GetCString.h"
#include "common/StringUtils.h"

#include <string>

namespace ureport
{
namespace details
{
    class FreshZipFile : public BasicZipFile
    {
        std::string m_Path;

    public:
        FreshZipFile(const std::string& path)
            : m_Path(path)
        {
            m_File = OpenFile(m_Path);
            m_Archive = OpenZipFile(m_File.get());
        }

        std::string GetPath() const
        {
            return m_Path;
        }

    private:
        static std::shared_ptr<FILE> OpenFile(const std::string& path);

        static std::shared_ptr<void> OpenZipFile(FILE* file);
    };
}
}
