#pragma once

#include "reporter/Sender.h"

namespace ureport
{
namespace test
{
    class SenderSpy : public Sender
    {
        unsigned int& m_SendCounter;

    public:
        SenderSpy(unsigned int& sendCounter)
            : m_SendCounter(sendCounter)
        {
        }

        void Send(const UnityBugMessage&, LongTermOperObserver*)
        {
            ++m_SendCounter;
        }
    };
}
}
