#include "ReportActionThread.h"
#include "reporter/ComposeReportController.h"
#include "long_term_operation/LongTermOperObserver.h"
#include "progression/Progression.h"
#include "ProgressionOperationObserver.h"

namespace ureport
{
    void ReportActionThread::SetReportController(std::shared_ptr<ComposeReportController> controller)
    {
        m_Controller = controller;
    }

    void ReportActionThread::run()
    {
        m_Canceled = false;
        try
        {
            DoAction(*m_Controller);
            if (m_Canceled)
                return;
            emit Completed();
        }
        catch (const std::exception& e)
        {
            emit Failed(QString::fromStdString(e.what()));
        }
    }

    void ReportActionThread::ReportProgress(size_t current, size_t total)
    {
        if (!m_Canceled)
            emit ProgressChanged(current, total);
    }

    void ReportActionThread::ReportStatus(const std::string& status)
    {
        if (!m_Canceled)
            emit StatusChanged(QString::fromStdString(status));
    }

    bool ReportActionThread::IsOperationCanceled() const
    {
        return m_Canceled;
    }

    void ReportActionThread::Cancel()
    {
        m_Canceled = true;
    }
}
