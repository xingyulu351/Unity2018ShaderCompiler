#include "SendReportThread.h"
#include "reporter/ComposeReportController.h"
#include "long_term_operation/LongTermOperObserver.h"
#include "progression/Progression.h"
#include "ProgressionOperationObserver.h"

namespace ureport
{
    void SendReportThread::DoAction(ComposeReportController& controller)
    {
        unity::progression::Progression progression;
        ProgressionOperationObserver observer(progression, *this);
        controller.PrepareReport(&observer);
        if (IsOperationCanceled())
            return;
        controller.SendReport(&observer);
    }
}
