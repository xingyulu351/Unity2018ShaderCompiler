#include "SaveReportThread.h"
#include "reporter/ComposeReportController.h"
#include "long_term_operation/LongTermOperObserver.h"
#include "progression/Progression.h"
#include "ProgressionOperationObserver.h"

namespace ureport
{
    void SaveReportThread::DoAction(ComposeReportController& controller)
    {
        unity::progression::Progression progression;
        ProgressionOperationObserver observer(progression, *this);
        controller.PrepareReport(&observer);
        if (IsOperationCanceled())
            return;
        controller.AttachReportData();
        if (IsOperationCanceled())
            return;
        controller.SaveReport(&observer, m_fileName);
    }

    std::string SaveReportThread::GetFileName() const
    {
        return m_fileName;
    }

    void SaveReportThread::SetFileName(const std::string& fileName)
    {
        m_fileName = fileName;
    }
}
