#pragma once

#include "reporter/Sender.h"
#include "reporter/UnityBugMessage.h"

#include <string>

namespace ureport
{
namespace test
{
    class SenderReportSpy : public ureport::Sender
    {
    public:
        std::string m_MessageEmail;
        std::string m_MessageEvent;

        virtual void Send(const UnityBugMessage& message, LongTermOperObserver*)
        {
            m_MessageEvent = message.GetEvent();
            m_MessageEmail = message.GetCustomerEmail();
        }
    };
}
}
