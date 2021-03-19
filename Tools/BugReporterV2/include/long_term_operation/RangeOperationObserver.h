#pragma once

#include "LongTermOperObserver.h"

#include <memory>

namespace ureport
{
    class RangeOperationObserver
    {
        std::shared_ptr<LongTermOperObserver> m_Original;
        size_t m_LowerBound;
        size_t m_UpperBound;

    public:
        RangeOperationObserver(std::shared_ptr<LongTermOperObserver> original)
            : m_Original(original)
        {
        }

        void ReportStatus(const std::string& status)
        {
            m_Original->ReportStatus(status);
        }

        void ReportProgress(size_t current, size_t total)
        {
            if (total == 0)
                return;
            const float fraction = float(current) / total;
            const auto distance = GetUpperBound() - GetLowerBound();
            m_Original->ReportProgress(GetLowerBound() + size_t(distance * fraction), total);
        }

        bool IsOperationCanceled() const
        {
            return m_Original->IsOperationCanceled();
        }

        size_t GetLowerBound() const
        {
            return m_LowerBound;
        }

        size_t GetUpperBound() const
        {
            return m_UpperBound;
        }

        void SetRange(size_t lower, size_t upper)
        {
            m_LowerBound = lower;
            m_UpperBound = upper;
        }
    };
}
