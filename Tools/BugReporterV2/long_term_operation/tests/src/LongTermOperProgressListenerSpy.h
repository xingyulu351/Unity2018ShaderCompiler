#pragma once

#include "long_term_operation/LongTermOperProgressListener.h"

namespace ureport
{
namespace test
{
    class LongTermOperProgressListenerSpy : public ureport::LongTermOperProgressListener
    {
    public:
        size_t m_Current;
        size_t m_Total;
        std::string m_Status;

        LongTermOperProgressListenerSpy()
            : m_Current(-1)
            , m_Total(-1)
            , m_Status("empty")
        {}

    private:
        void UpdateProgress(size_t current, size_t total)
        {
            m_Current = current;
            m_Total = total;
        }

        void UpdateStatus(const std::string& status)
        {
            m_Status = status;
        }
    };
}
}
