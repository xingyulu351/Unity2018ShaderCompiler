#pragma once

#include "FileSystemEntry.h"
#include "FileSystem.h"

#include <vector>
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>

namespace ureport
{
namespace details
{
    class ProjectDirEntry : public FileSystemEntry
    {
    public:
        ProjectDirEntry(const std::string& path, std::shared_ptr<FileSystem> filesystem)
            : m_FileSystem(filesystem)
            , m_Path(path)
        {
            m_TempNames.push_back(path + "/Temp");
            m_TempNames.push_back(path + "/Library/GiCache");
            m_TempNames.push_back(path + "/Library/ShaderCache");
        }

        virtual std::list<std::string> GetEntries(const std::string& filter) const override
        {
            const auto allEntries = m_FileSystem->GetDirEntries(m_Path, filter);
            std::list<std::string> validEntries;
            std::copy_if(allEntries.begin(), allEntries.end(),
                std::back_inserter(validEntries), [this] (const std::string& entry)
                {
                    return !IsTempEntry(entry);
                });
            return validEntries;
        }

        virtual std::string GetPath() const override
        {
            return m_Path;
        }

        virtual bool IsDir() const override
        {
            return true;
        }

        virtual size_t GetSize() const override
        {
            auto const files = GetFiles();
            return std::accumulate(files.begin(), files.end(), size_t(0),
                [&] (size_t size, const std::string& file) {
                    return size + m_FileSystem->GetSize(file);
                });
        }

    private:
        bool IsTempEntry(const std::string& entry) const
        {
            return std::any_of(m_TempNames.begin(), m_TempNames.end(),
                [&entry] (const std::string& name)
                {
                    return entry.find(name) != std::string::npos;
                });
        }

        std::vector<std::string> GetFiles() const
        {
            auto const entries = GetEntries("");
            std::vector<std::string> files;
            std::copy_if(entries.begin(), entries.end(),
                std::back_inserter(files), [this] (const std::string& entry)
                {
                    return !m_FileSystem->IsDir(entry);
                });
            return files;
        }

    private:
        std::shared_ptr<FileSystem> m_FileSystem;
        std::string m_Path;
        std::vector<std::string> m_TempNames;
    };
}
}
