#pragma once

#include "reporter/Collector.h"

namespace ureport
{
namespace test
{
    class CollectorCallSpy : public Collector
    {
        mutable unsigned int m_Count;

    public:
        CollectorCallSpy()
            : m_Count(0)
        {
        }

        unsigned int GetCallCount() const
        {
            return m_Count;
        }

        void Collect(ReportDraft& report) const
        {
            ++m_Count;
        }
    };
}
}
