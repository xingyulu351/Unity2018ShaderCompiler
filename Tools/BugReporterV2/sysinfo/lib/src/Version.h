#pragma once

#include <cstdint>
#include <string>
#include <sstream>

namespace ureport
{
    class Version
    {
        uint32_t m_Major;
        uint32_t m_Minor;
        uint32_t m_Build;

    public:
        Version()
            : m_Major(0)
            , m_Minor(0)
            , m_Build(0)
        {
        }

        Version(uint32_t major, uint32_t minor)
            : m_Major(major)
            , m_Minor(minor)
            , m_Build(0)
        {
        }

        Version(uint32_t major, uint32_t minor, uint32_t build)
            : m_Major(major)
            , m_Minor(minor)
            , m_Build(build)
        {
        }

        uint32_t GetMajor() const
        {
            return m_Major;
        }

        uint32_t GetMinor() const
        {
            return m_Minor;
        }

        uint32_t GetBuild() const
        {
            return m_Build;
        }

        std::string GetTextualForm() const
        {
            std::stringstream text;
            text << m_Major << '.' << m_Minor << '.' << m_Build;
            return text.str();
        }
    };
}
