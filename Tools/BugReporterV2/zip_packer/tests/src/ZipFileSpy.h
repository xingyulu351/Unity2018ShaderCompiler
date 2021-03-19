#pragma once

#include "ZipFile.h"

#include <istream>
#include <iterator>
#include <algorithm>

namespace ureport
{
namespace test
{
    class ZipFileSpy : public ureport::ZipFile
    {
        std::string m_EntryName;
        std::string m_EntryContent;

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
            m_EntryName = name;
        }

        std::unique_ptr<std::istream> Read() const
        {
            return std::unique_ptr<std::istream>();
        }

        void Write(std::istream& data)
        {
            using namespace std;
            copy(istream_iterator<char>(data), istream_iterator<char>(),
                inserter(m_EntryContent, m_EntryContent.begin()));
        }

    public:
        std::string GetEntryName() const
        {
            return m_EntryName;
        }

        std::string GetEntryContent() const
        {
            return m_EntryContent;
        }
    };
}
}
