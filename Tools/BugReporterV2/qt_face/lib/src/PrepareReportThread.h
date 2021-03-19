#pragma once

#include "ReportActionThread.h"

namespace ureport
{
    class PrepareReportThread : public ReportActionThread
    {
        void DoAction(ComposeReportController& controller) override;
    };
}
