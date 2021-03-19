#pragma once

#include "long_term_operation/LongTermOperObserver.h"

#include <string>

namespace ureport
{
namespace test
{
    class OperationObserverSpy : public ureport::LongTermOperObserver
    {
    public:
        std::string m_Status;
        size_t m_Progress;
        bool m_Canceled;

        void ReportProgress(size_t current, size_t total) override
        {
            m_Progress = current;
        }

        void ReportStatus(const std::string& status) override
        {
            m_Status = status;
        }

        bool IsOperationCanceled() const override
        {
            return m_Canceled;
        }
    };
}
}
