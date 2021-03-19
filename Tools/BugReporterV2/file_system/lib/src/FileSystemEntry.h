#pragma once

#include <memory>
#include <string>
#include <list>
#include <functional>
#include <algorithm>

namespace ureport
{
    class FileSystemEntry
    {
    public:
        virtual std::list<std::string> GetEntries(const std::string& filter) const = 0;
        virtual std::string GetPath() const = 0;
        virtual size_t GetSize() const = 0;
        virtual bool IsDir() const = 0;
        virtual ~FileSystemEntry() {}

        void ProcessEntries(const std::string filter,
            std::function<void(const std::string&)> process) const
        {
            const auto& entries = GetEntries(filter);
            std::for_each(entries.begin(), entries.end(), process);
        }
    };
}
