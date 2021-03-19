#pragma once

#include "ReportModifier.h"

namespace ureport
{
    class ReportDraft;

    class Collector : public ReportModifier
    {
    public:
        virtual void Collect(ReportDraft& report) const = 0;

        virtual ~Collector() {}

    private:
        void Modify(ReportDraft& report) const
        {
            Collect(report);
        }
    };
}
