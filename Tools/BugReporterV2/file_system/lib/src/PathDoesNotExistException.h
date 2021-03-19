#pragma once

#include <stdexcept>
#include <exception>
#include <string>

namespace ureport
{
    class PathDoesNotExistException : public std::logic_error
    {
    public:
        PathDoesNotExistException(const std::string& path)
            : std::logic_error("Path " + path + " does not exist")
            , m_Path(path)
        {
        }

        std::string GetPath() const
        {
            return m_Path;
        }

    private:
        std::string m_Path;
    };
}
