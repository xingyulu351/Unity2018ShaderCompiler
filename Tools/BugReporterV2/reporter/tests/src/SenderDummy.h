#pragma once

#include "reporter/Sender.h"

namespace ureport
{
namespace test
{
    class SenderDummy : public Sender
    {
        void Send(const UnityBugMessage&, LongTermOperObserver*)
        {
        }
    };
}
}
