#pragma once

#include <vector>
#include <memory>
#include <functional>

namespace ureport
{
    class Report;
    class ReportDraft;
    class LongTermOperObserver;

    class ReportWorksheet
    {
    public:
        virtual bool FillReport(
            std::shared_ptr<ReportDraft> report,
            std::function<void(ReportDraft&, LongTermOperObserver*)> prepareReport,
            std::function<void(Report&, LongTermOperObserver*)> sendReport,
            std::function<void(Report&, LongTermOperObserver*, const std::string&)> saveReport) = 0;
    };
}
