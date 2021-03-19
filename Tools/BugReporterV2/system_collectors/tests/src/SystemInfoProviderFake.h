#pragma once

#include "SystemInfoProviderDummy.h"

namespace ureport
{
namespace test
{
    class OSNameSystemInfoProviderFake : public SystemInfoProviderDummy
    {
        std::string m_OSName;

    public:
        OSNameSystemInfoProviderFake(const std::string& name)
            : m_OSName(name)
        {
        }

        std::string GetOSName() const
        {
            return m_OSName;
        }
    };

    class OSVersionSystemInfoProviderFake : public SystemInfoProviderDummy
    {
        Version m_OSVersion;

    public:
        OSVersionSystemInfoProviderFake(const Version& version)
            : m_OSVersion(version)
        {
        }

        Version GetOSVersion() const
        {
            return m_OSVersion;
        }
    };
}
}
