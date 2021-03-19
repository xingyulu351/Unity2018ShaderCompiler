#pragma once

#include "reporter/ReportMonitor.h"

namespace ureport
{
    class ReportChangedMonitor : public ureport::ReportMonitor
    {
        bool m_IsReportChanged;
        bool m_IsBlocked;

    public:
        ReportChangedMonitor()
            : m_IsReportChanged(false)
            , m_IsBlocked(false)
        {
        }

        bool IsReportChanged() const
        {
            return m_IsReportChanged;
        }

        void ReportUpdated()
        {
            if (!m_IsBlocked)
                m_IsReportChanged = true;
        }

        void Block()
        {
            m_IsBlocked = true;
        }

        void UnBlock()
        {
            m_IsBlocked = false;
        }

        void Reset()
        {
            m_IsReportChanged = false;
        }
    };
}
