#pragma once

#include "long_term_operation/LongTermOperObserver.h"

#include <QThread>
#include <memory>

namespace ureport
{
    class ComposeReportController;
    class LongTermOperObserver;

    class ReportActionThread : public QThread, public LongTermOperObserver
    {
        Q_OBJECT

    public:
        void SetReportController(std::shared_ptr<ComposeReportController> controller);

        void run() override;

        void ReportProgress(size_t current, size_t total) override;

        void ReportStatus(const std::string& status) override;

        bool IsOperationCanceled() const override;

    signals:
        void StatusChanged(const QString& status);

        void ProgressChanged(size_t current, size_t total);

        void Completed();

        void Failed(const QString& error);

    private slots:
        void Cancel();

    protected:
        virtual void DoAction(ComposeReportController& controller) = 0;

    private:
        std::shared_ptr<ComposeReportController> m_Controller;
        bool m_Canceled;
    };
}
