#pragma once

#include "HardwareInfoProviderDummy.h"

#include <vector>
#include <algorithm>
#include <iterator>

namespace ureport
{
namespace test
{
    class BasicHardwareTypeProviderFake : public HardwareInfoProviderDummy
    {
        std::string m_Value;
    public:
        BasicHardwareTypeProviderFake(const std::string& basicHardwareType)
            : m_Value(basicHardwareType)
        {
        }

        std::string GetBasicHardwareType() const
        {
            return m_Value;
        }
    };

    class GraphicsHardwareTypeProviderFake : public HardwareInfoProviderDummy
    {
        std::string m_Value;
    public:
        GraphicsHardwareTypeProviderFake(const std::string& graphicsHardwareType)
            : m_Value(graphicsHardwareType)
        {
        }

        std::string GetGraphicsHardwareType() const
        {
            return m_Value;
        }
    };
}
}
