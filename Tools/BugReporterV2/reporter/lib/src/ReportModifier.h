#pragma once

#include <memory>
#include <functional>

namespace ureport
{
    class ReportDraft;

    class ReportModifier
    {
    public:
        virtual void Modify(ReportDraft& report) const = 0;

        virtual ~ReportModifier() {}
    };

    std::shared_ptr<ReportModifier> MakeReportModifier(
        std::function<void(ReportDraft&)> action);
}
