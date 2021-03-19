#include "MainWindow.h"
#include "StyleSheets.h"
#include "CrashSolutionCheckDialog.h"
#include "reporter/ReportWorksheet.h"
#include "reporter/ReportDraft.h"
#include "reporter/ComposeReportController.h"
#include "attachment/AttachmentFactory.h"
#include "attachment/Attachment.h"
#include "file_system/SystemFileGateway.h"
#include "file_system/File.h"
#include "file_system/FileSystem.h"
#include "file_system/FileSystemEntry.h"
#include "search_integration/SimilarCasesWebSearch.h"
#include "attachment/AttachmentProperties.h"
#include "feedback/BasicFeedbackManager.h"

#include <QApplication>
#include <QDir>
#include <QTranslator>
#include <memory>
#include <algorithm>
#include <iostream>

int NO_ARGS_C = 0;
char* NO_ARGS_V[1] = {};

namespace ureport
{
namespace qt_face
{
    class ReportWorksheet : public ureport::ReportWorksheet
    {
        std::unique_ptr<QApplication> m_App;
        std::shared_ptr<MainWindow> m_Form;
        std::shared_ptr<FileGateway> m_FilesGateway;
        std::shared_ptr<FileSystem> m_FileSystem;

    public:
        ReportWorksheet(
            std::shared_ptr<FileGateway> fileGateway,
            std::shared_ptr<FileSystem> fileSystem)
            : m_App(makeApplication())
            , m_Form(new MainWindow())
            , m_FilesGateway(fileGateway)
            , m_FileSystem(fileSystem)
        {
        }

        static QString GetExecutableDirPath()
        {
            QCoreApplication tempApp(NO_ARGS_C, NO_ARGS_V);
            return QCoreApplication::applicationDirPath();
        }

        static std::unique_ptr<QApplication> makeApplication()
        {
            QCoreApplication::setLibraryPaths(QStringList(GetExecutableDirPath()));
            std::unique_ptr<QApplication> app(new QApplication(NO_ARGS_C, NO_ARGS_V));
            auto translator = new QTranslator(app.get());
            QString localeName = QLocale::system().name();
            translator->load(QString("bugreporter_") + localeName, QCoreApplication::applicationDirPath() + "/localization/");
            app->installTranslator(translator);
            return app;
        }

        bool FillReport(
            std::shared_ptr<ReportDraft> report,
            std::function<void(ReportDraft&, LongTermOperObserver*)> prepareReport,
            std::function<void(Report&, LongTermOperObserver*)> sendReport,
            std::function<void(Report&, LongTermOperObserver*, const std::string&)> saveReport)
        {
            ComposeReport(report, prepareReport, sendReport, saveReport);
            bool filled = m_Form->IsReportFilledUp();
            m_Form.reset();
            m_App.reset();
            return filled;
        }

        void ComposeReport(
            std::shared_ptr<ReportDraft> report,
            std::function<void(ReportDraft&, LongTermOperObserver*)> prepareReport,
            std::function<void(Report&, LongTermOperObserver*)> sendReport,
            std::function<void(Report&, LongTermOperObserver*, const std::string&)> saveReport)
        {
            std::shared_ptr<ComposeReportController> controller =
                CreateComposeReportController(
                    CreateAttachmentFactory(m_FileSystem, m_FilesGateway),
                    CreateFileSystem(),
                    prepareReport, sendReport, saveReport);
            controller->SetReport(report);

            auto crashAnalyzer = std::shared_ptr<CrashAnalyzer>(new CrashAnalyzer("https://crashes-collector.unity3d.com", report));
            if (report->GetBugTypeID() == ureport::Report::kBugCrashAuto && crashAnalyzer->GetServiceStatus() == ureport::qt_face::CrashServiceStatus::Available)
            {
                auto crashCheckDialog = new CrashSolutionCheckDialog(NULL, crashAnalyzer);
                auto checkDialogResult = crashCheckDialog->exec();
                if (checkDialogResult == QDialog::Rejected)
                {
                    return;
                }
            }

            m_Form->SetReportController(controller);
            m_Form->SetReport(report);
            m_Form->SetSearch(std::unique_ptr<SimilarCasesWebSearch>(CreateSimilarCasesWebSearch()));
            m_Form->SetFeedbackManager(CreateFeedbackManager());
            ExecuteForm();
        }

        void ExecuteForm()
        {
            m_Form->show();
            m_App->exec();
        }
    };

    std::unique_ptr<ureport::ReportWorksheet> CreateWorksheet()
    {
        return std::unique_ptr<ureport::ReportWorksheet>(
            new ReportWorksheet(CreateSystemFileGateway(), CreateFileSystem()));
    }
}
}
