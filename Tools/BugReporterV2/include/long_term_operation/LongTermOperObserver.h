#pragma once

#include <string>

namespace ureport
{
    class LongTermOperObserver
    {
    public:
        virtual void ReportProgress(size_t current, size_t total) = 0;
        virtual void ReportStatus(const std::string& status) = 0;
        virtual bool IsOperationCanceled() const = 0;
        virtual ~LongTermOperObserver() {}
    };

    class LongTermBlockedOperObserver
    {
    public:
        LongTermBlockedOperObserver(LongTermOperObserver* observer)
            : m_Observer(observer)
        {
        }

        void ReportProgress(size_t current, size_t total)
        {
            if (m_Observer)
                m_Observer->ReportProgress(current, total);
        }

        void ReportStatus(const std::string& status)
        {
            if (m_Observer)
                m_Observer->ReportStatus(status);
        }

        bool IsOperationCanceled() const
        {
            return m_Observer ? m_Observer->IsOperationCanceled() : false;
        }

    private:
        LongTermOperObserver* m_Observer;

        LongTermBlockedOperObserver(const LongTermBlockedOperObserver&);
    };
}
