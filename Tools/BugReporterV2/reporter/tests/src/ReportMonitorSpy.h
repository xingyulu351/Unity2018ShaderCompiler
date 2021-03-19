#pragma once

#include "reporter/ReportMonitor.h"

namespace ureport
{
namespace test
{
    class ReportMonitorSpy : public ureport::ReportMonitor
    {
        unsigned int m_Count;

    public:
        ReportMonitorSpy()
            : m_Count(0)
        {
        }

        unsigned int GetUpdateCount() const
        {
            return m_Count;
        }

    private:
        void ReportUpdated()
        {
            ++m_Count;
        }
    };
}
}
