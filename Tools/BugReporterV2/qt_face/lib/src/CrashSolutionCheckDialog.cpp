#include "CrashSolutionCheckDialog.h"
#include "reporter/ReportDraft.h"
#include "ui_CrashSolutionCheckDialog.h"

#include <QMovie>
#include <QThread>
#include <QMessageBox>

CrashSolutionCheckDialog::CrashSolutionCheckDialog(QWidget *parent, std::shared_ptr<ureport::qt_face::CrashAnalyzer> crashAnalyzer) :
    QDialog(parent),
    ui(new Ui::CrashSolutionCheckDialog)
{
    ui->setupUi(this);
    connect(ui->reportBugButton, SIGNAL(clicked(bool)), this, SLOT(OnReportBugButtonClicked()));
    connect(ui->closeButton, SIGNAL(clicked(bool)), this, SLOT(OnCloseButtonClicked()));
    connect(this, SIGNAL(finished(int)), SLOT(OnCrashSolutionCheckDialogFinished(int)));

    QMovie* progressMovie = new QMovie(":/new/prefix1/progress.gif");
    ui->progressBarLabel->setMovie(progressMovie);

    thread = new Thread(crashAnalyzer);
    connect(thread, SIGNAL(finished(QJsonObject)), SLOT(OnWorkCompleted(QJsonObject)));

    SetCheckingState(Checking);
    thread->start();
}

CrashSolutionCheckDialog::~CrashSolutionCheckDialog()
{
    delete thread;
    delete ui;
}

void CrashSolutionCheckDialog::SetCheckingState(CheckingStatus checkingStatus)
{
    status = checkingStatus;
    QString statusString = Convert(checkingStatus);
    ui->statusLabel->setText(statusString);
    bool isChecking = checkingStatus == Checking;
    ui->progressBarLabel->setVisible(isChecking);
    ui->progressBarLabel->movie()->setPaused(!isChecking);
    ui->reportBugButton->setEnabled(!isChecking);
}

QString CrashSolutionCheckDialog::Convert(CheckingStatus status)
{
    switch (status)
    {
        case NotChecked:
            return QString("Not Checked");
        case Checking:
            return QString("Checking...");
        case SolutionFound:
        {
            auto fixedBuildUrl = crashReportResponse["fix_url"].toString();
            auto buildVersion = crashReportResponse["fixed_in"].toString();
            return QString("There is a fix for the crash. Please download Unity <a href='%1'>version %2</a>.").arg(fixedBuildUrl).arg(buildVersion);
        }
        case KnownCrash:
        {
            auto issueTrackerUrl = crashReportResponse["issue_tracker_url"].toString();
            return QString("This is a known crash. Please find more info on the <a href='%1'>Issue Tracker</a>.").arg(issueTrackerUrl);
        }
        case UnknownCrash:
            return QString("There is no fix available. Please report a bug.");
        default:
            return QString("Server is unavailable");
    }
}

void CrashSolutionCheckDialog::OnWorkCompleted(QJsonObject result)
{
    crashReportResponse = result;
    auto responseStatus = UnknownCrash;
    if (result.contains("error"))
    {
        responseStatus = Failed;
    }
    else if (result.contains("fixed_in"))
    {
        responseStatus = SolutionFound;
    }
    else if (result.contains("issue_tracker_url"))
    {
        responseStatus = KnownCrash;
    }
    SetCheckingState(responseStatus);
}

void CrashSolutionCheckDialog::OnReportBugButtonClicked()
{
    accept();
}

void CrashSolutionCheckDialog::OnCloseButtonClicked()
{
    reject();
}

void CrashSolutionCheckDialog::OnCrashSolutionCheckDialogFinished(int result)
{
    if (thread->isRunning())
    {
        thread->stop();
        thread->wait();
    }
}
