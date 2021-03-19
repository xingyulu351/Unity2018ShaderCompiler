#pragma once

#include <memory>

namespace ureport
{
    class UnityBugMessage;
    class LongTermOperObserver;

    class Sender
    {
    public:
        virtual void Send(const UnityBugMessage& message, LongTermOperObserver*) = 0;

        virtual ~Sender() {}
    };
}
