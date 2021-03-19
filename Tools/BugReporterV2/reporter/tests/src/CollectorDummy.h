#pragma once

#include "reporter/Collector.h"

namespace ureport
{
namespace test
{
    class CollectorDummy : public Collector
    {
        void Collect(ReportDraft& report) const
        {
        }
    };
}
}
