#pragma once

#include "ReportActionThread.h"

namespace ureport
{
    class SendReportThread : public ReportActionThread
    {
        void DoAction(ComposeReportController& controller) override;
    };
}
