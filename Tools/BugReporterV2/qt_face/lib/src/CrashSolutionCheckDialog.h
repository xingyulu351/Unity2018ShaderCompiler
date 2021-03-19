#pragma once

#include <QDialog>
#include <QThread>
#include "CrashAnalyzer.h"
#include "reporter/ReportDraft.h"

namespace Ui
{
    class CrashSolutionCheckDialog;
}

namespace ureport
{
    class ReportDraft;

namespace qt_face
{
    class CrashAnalyzer;
}
}

enum CheckingStatus { NotChecked = 0, Checking, UnknownCrash, KnownCrash, SolutionFound, Failed };

class Thread : public QThread
{
    Q_OBJECT

public:
    Thread(std::shared_ptr<ureport::qt_face::CrashAnalyzer> crashAnalyzer)
    {
        m_CrashAnalyzer = crashAnalyzer;
        stopped = false;
    }

    void stop()
    {
        stopped = true;
    }

protected:
    void run()
    {
        auto crashReport = m_CrashAnalyzer->GetReport();
        QThread::msleep(1000);
        stopped = false;
        emit finished(crashReport);
    }

signals:
    void finished(QJsonObject);

private:
    volatile bool stopped;
    std::shared_ptr<ureport::qt_face::CrashAnalyzer> m_CrashAnalyzer;
};


class CrashSolutionCheckDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CrashSolutionCheckDialog(QWidget *parent = 0, std::shared_ptr<ureport::qt_face::CrashAnalyzer> crashAnalyzer = 0);
    ~CrashSolutionCheckDialog();

private slots:
    void OnWorkCompleted(QJsonObject);
    void OnReportBugButtonClicked();
    void OnCloseButtonClicked();
    void OnCrashSolutionCheckDialogFinished(int);

private:
    Ui::CrashSolutionCheckDialog *ui;
    void SetCheckingState(CheckingStatus);
    QString Convert(CheckingStatus);
    Thread *thread;
    CheckingStatus status;
    QJsonObject crashReportResponse;
};
