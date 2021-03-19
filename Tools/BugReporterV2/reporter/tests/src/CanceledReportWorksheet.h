#pragma once

#include "reporter/ReportWorksheet.h"

namespace ureport
{
namespace test
{
    class CanceledReportWorksheet : public ReportWorksheet
    {
        bool FillReport(
            std::shared_ptr<ReportDraft> report,
            std::function<void(ReportDraft&, LongTermOperObserver*)> prepareReport,
            std::function<void(Report&, LongTermOperObserver*)> sendReport,
            std::function<void(Report&, LongTermOperObserver*, const std::string&)> saveReport)
        {
            return false;
        }
    };
}
}
