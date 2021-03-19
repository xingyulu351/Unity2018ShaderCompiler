#pragma once

#include "progression/Monitor.h"

namespace unity
{
namespace progression
{
namespace test
{
    class MonitorSpy : public progression::Monitor
    {
    public:
        size_t m_ProgressChangeCount;

        MonitorSpy()
            : m_ProgressChangeCount(0)
        {
        }

    private:
        void ProgressChanged() override
        {
            ++m_ProgressChangeCount;
        }
    };
}
}
}
