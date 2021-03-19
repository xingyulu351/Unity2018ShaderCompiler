#include "SendReportThread.h"
#include "SaveReportThread.h"
#include "PrepareReportThread.h"
#include "search_integration/SearchResult.h"
#include "alert.h"

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QProgressDialog>
#include <QtWidgets>
#include <memory>

namespace Ui
{
    class MainWindow;
}

namespace ureport
{
    class Report;
    class ComposeReportController;
    class LongTermOperController;
    class LongTermOperObserver;
    class ReportChangedMonitor;
    class SimilarCasesSearch;
    class FeedbackManager;

namespace ui
{
    class AttachmentsTableModel;
}
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    void SetReportTitle(const QString& title);

    QString GetReportTitle() const;

    void SetAdditionalInfo(const QString& text);

    QString GetAdditionalInfo() const;

    void SetReporterEmail(const QString& email);

    QString GetReporterEmail() const;

    bool IsReportFilledUp() const;

    void SetReport(std::shared_ptr<ureport::Report> report);

    void SetReportController(
        std::shared_ptr<ureport::ComposeReportController> controller);

    void SetSearch(std::unique_ptr<ureport::SimilarCasesSearch> search);

    void SetFeedbackManager(std::unique_ptr<ureport::FeedbackManager> manager);

protected:
    virtual void resizeEvent(QResizeEvent * ev);

private:
    void AddListItem(QString title, QString url);

    void UpdateView(const ureport::Report& report);

    void UpdateAttachmentView(const ureport::Report& report);

    void closeEvent(QCloseEvent* event);

    void FixBugType();

    bool ConfirmExit();

    void ShowSuccessConfirmation();

    void ShowSaveConfirmation();

    void SetupProgressDialog();

    void ShowErrorMessage(const QString& message);

    QString GetDefaultTextualDescription() const;

    void SetControlsEnabled(bool state);

    QPixmap GetIcon(const QString& resourceName) const;

    void UpdateDescriptionLength(int descriptionLength);

    void SaveWindowState() const;

    void RestoreWindowState();

    void SetupProblemComboBoxItems();

    void SetupReproComboBoxItems();

    int GetDefaultTextualDescriptionLength() const;

    void SetupSearchPopup();

    void SetupFeedbackChecking();

    void AdjustSearchPopupPosition();

    void SetupSearchProgressBar();

    void PopulateSearchResultsItems(
        std::shared_ptr<ureport::SearchResult> result);

    void PolulateListWithStaticItems();

    void AdjustSearchProgressBarPosition();

    void ShowSearchProgressBar();

    void HideSearchProgressBar();

    void attachmentViewSelectionChanged(const QItemSelection& selected,
        const QItemSelection& deselected);

    static void AdjustWidgetsPosition(QWidget* parentWidget, QWidget* widget, int size);

    void RefreshSendButton();

private slots:

    void on_attachFileButton_clicked();

    void on_attachFolderButton_clicked();

    void on_removeFileButton_clicked();

    void on_cancelButton_clicked();

    void on_previewButton_clicked();

    void on_sendButton_clicked();

    void on_saveButton_clicked();

    void OnTitleChanged(const QString& text);

    void OnAdditionalInfoChanged();

    void OnEmailChanged(const QString& text);

    void OnBugTypeIDChanged(const int index);

    void OnBugReproChanged(const int index);

    void OnPrepareStarted();

    void OnPrepareDone();

    void OnPrepareFinished();

    void OnPrepareFailed(const QString& error);

    void OnSendStarted();

    void OnReportStatusChanged(const QString& status);

    void OnReportProgressChanged(size_t current, size_t total);

    void OnSendDone();

    void OnSendFailed(const QString& error);

    void OnSendFinished();

    void OnSaveStarted();

    void OnSaveDone();

    void OnSaveFailed(const QString& error);

    void OnSaveFinished();

    void FindSimilarCases();

    void GetFeedback();

    void ShowSearchResults(std::shared_ptr<ureport::SearchResult> result);

private:
    static const int WARNING_CHARS_LIMIT = 100;
    static const int INDICATOR_AREA_SIZE = 20;
    Ui::MainWindow* ui;
    bool m_IsFilledUp;
    bool m_BlockSendingByFeedback;
    bool m_BlockSendingByDescription;
    bool m_BlockSendingByControls;
    std::shared_ptr<ureport::ui::AttachmentsTableModel> m_AttachmentsModel;
    std::shared_ptr<QSortFilterProxyModel> m_AttachmentsProxyModel;
    std::shared_ptr<ureport::Report> m_Report;
    std::shared_ptr<ureport::ComposeReportController> m_ReportController;
    std::shared_ptr<ureport::ReportChangedMonitor> m_ReportChangedMonitor;
    ureport::SendReportThread m_SendReportThread;
    ureport::PrepareReportThread m_PrepareReportThread;
    ureport::SaveReportThread m_SaveReportThread;
    QProgressDialog m_ProgressDialog;
    QTimer *m_SearchTimer;
    QTimer *m_FeedbackTimer;
    QLabel *m_ProgressIndicator;
    std::unique_ptr<ureport::SimilarCasesSearch> m_Search;
    ureport::ui::Alert *alert;
    std::unique_ptr<ureport::FeedbackManager> m_FeedbackManager;
    bool m_AutoCrash;
    bool m_DocumentationIssue;
    bool m_SendEnabled;
};
