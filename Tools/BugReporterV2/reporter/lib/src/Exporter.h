#pragma once

#include "Report.h"

#include <memory>

namespace ureport
{
    class UnityBugMessage;
    class LongTermOperObserver;

    class Exporter
    {
    public:

        virtual void Save(const UnityBugMessage& message, LongTermOperObserver*, const std::string& path) = 0;

        virtual ~Exporter() {}
    };
}
