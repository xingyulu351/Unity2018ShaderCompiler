#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "PreviewDialog.h"
#include "SignalBlocker.h"
#include "AttachmentsTableModel.h"
#include "reporter/ComposeReportController.h"
#include "reporter/ReportChangedMonitor.h"
#include "attachment/Attachment.h"
#include "attachment_preview/AttachmentTextPreview.h"
#include "settings/Settings.h"
#include "shims/logical/IsEmpty.h"
#include "shims/logical/IsNull.h"
#include "common/CustomQtUtils.h"
#include "search_integration/SimilarCasesWebSearch.h"
#include "feedback/FeedbackManager.h"

#include <memory>
#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <sstream>

using namespace ::ureport;
using namespace ui;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    ,   ui(new Ui::MainWindow)
    , m_IsFilledUp(false)
    , m_AttachmentsModel(new AttachmentsTableModel(this))
    , m_AttachmentsProxyModel(new QSortFilterProxyModel(this))
    , m_ReportChangedMonitor(new ReportChangedMonitor())
    , m_SendReportThread()
    , m_SaveReportThread()
    , m_ProgressDialog(this)
    , m_BlockSendingByControls(false)
    , m_BlockSendingByFeedback(false)
    , m_AutoCrash(false)
    , m_DocumentationIssue(false)
    , m_BlockSendingByDescription(false)
{
    ui->setupUi(this);
    SetupProblemComboBoxItems();
    SetupReproComboBoxItems();
    QRegExp emailRegExp("[a-z0-9._%+-@]*",
                        Qt::CaseInsensitive);
    ui->emailEdit->setValidator(new QRegExpValidator(emailRegExp));
    m_AttachmentsProxyModel->setSourceModel(m_AttachmentsModel.get());
    m_AttachmentsProxyModel->setSortRole(AttachmentsTableModel::kSortRole);
    m_AttachmentsProxyModel->setFilterRole(AttachmentsTableModel::kFilterRole);
    m_AttachmentsProxyModel->setFilterFixedString(tr("removable"));

    SetupSearchPopup();
    SetupSearchProgressBar();


    auto settings = ureport::GetSettings();
    std::string sendUrl = settings->GetSendUrl();
    m_SendEnabled = (sendUrl != ureport::constants::kDisabled);
    ui->sendButton->setVisible(m_SendEnabled);
    ui->saveButton->setVisible(!m_SendEnabled);

    ui->attachedFilesView->setModel(m_AttachmentsProxyModel.get());
    ui->attachedFilesView->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft);
    ui->attachedFilesView->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::Stretch);
    ui->attachedFilesView->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::Fixed);
    ui->attachedFilesView->horizontalHeader()->setSectionResizeMode(
        2, QHeaderView::Fixed);
    ui->attachedFilesView->horizontalHeader()->setSectionHidden(2, true);
    ui->attachedFilesView->setSelectionBehavior(QAbstractItemView::SelectRows);

    SetupProgressDialog();
    RestoreWindowState();
    qRegisterMetaType<size_t>("size_t");
    connect(ui->titleEdit, SIGNAL(textChanged(const QString&)),
        SLOT(OnTitleChanged(const QString&)));
    connect(ui->descriptionEdit, SIGNAL(textChanged()),
        SLOT(OnAdditionalInfoChanged()));
    connect(ui->emailEdit, SIGNAL(textChanged(const QString&)),
        SLOT(OnEmailChanged(const QString&)));
    connect(ui->problemComboBox, SIGNAL(currentIndexChanged(int)),
        SLOT(OnBugTypeIDChanged(const int)));
    connect(ui->reproComboBox, SIGNAL(currentIndexChanged(int)),
        SLOT(OnBugReproChanged(const int)));
    connect(&m_PrepareReportThread, SIGNAL(started()),
        SLOT(OnPrepareStarted()));
    connect(&m_PrepareReportThread, SIGNAL(finished()),
        SLOT(OnPrepareFinished()));
    connect(&m_PrepareReportThread, SIGNAL(StatusChanged(const QString&)),
        SLOT(OnReportStatusChanged(const QString&)));
    connect(&m_PrepareReportThread, SIGNAL(ProgressChanged(size_t, size_t)),
        SLOT(OnReportProgressChanged(size_t, size_t)));
    connect(&m_PrepareReportThread, SIGNAL(Completed()),
        SLOT(OnPrepareDone()));
    connect(&m_PrepareReportThread, SIGNAL(Failed(const QString&)),
        SLOT(OnPrepareFailed(const QString&)));
    connect(&m_ProgressDialog, SIGNAL(canceled()), &m_PrepareReportThread,
        SLOT(Cancel()));
    connect(&m_SendReportThread, SIGNAL(started()), SLOT(OnSendStarted()));
    connect(&m_SendReportThread, SIGNAL(StatusChanged(const QString&)),
        SLOT(OnReportStatusChanged(const QString&)));
    connect(&m_SendReportThread, SIGNAL(ProgressChanged(size_t, size_t)),
        SLOT(OnReportProgressChanged(size_t, size_t)));
    connect(&m_SendReportThread, SIGNAL(Completed()), SLOT(OnSendDone()));
    connect(&m_SendReportThread, SIGNAL(Failed(const QString&)),
        SLOT(OnSendFailed(const QString&)));
    connect(&m_SendReportThread, SIGNAL(finished()), SLOT(OnSendFinished()));
    connect(&m_ProgressDialog, SIGNAL(canceled()), &m_SendReportThread,
        SLOT(Cancel()));
    connect(&m_SaveReportThread, SIGNAL(started()), SLOT(OnSaveStarted()));
    connect(&m_SaveReportThread, SIGNAL(StatusChanged(const QString&)),
        SLOT(OnReportStatusChanged(const QString&)));
    connect(&m_SaveReportThread, SIGNAL(ProgressChanged(size_t, size_t)),
        SLOT(OnReportProgressChanged(size_t, size_t)));
    connect(&m_SaveReportThread, SIGNAL(Completed()), SLOT(OnSaveDone()));
    connect(&m_SaveReportThread, SIGNAL(Failed(const QString&)),
        SLOT(OnSaveFailed(const QString&)));
    connect(&m_SaveReportThread, SIGNAL(finished()), SLOT(OnSaveFinished()));
    connect(&m_ProgressDialog, SIGNAL(canceled()), &m_SaveReportThread,
        SLOT(Cancel()));

    connect(ui->attachedFilesView->selectionModel(),
        &QItemSelectionModel::selectionChanged,
        this, &MainWindow::attachmentViewSelectionChanged); ui->titleEdit->setFocus();

    ui->titleEdit->installEventFilter(this);
    ui->titleEdit->setTextMargins(ui->titleEdit->textMargins().left(),
        ui->titleEdit->textMargins().top(),
        ui->titleEdit->textMargins().right() + INDICATOR_AREA_SIZE,
        ui->titleEdit->textMargins().bottom());

    alert = new Alert(ui->notificationLabel, ui->ratingLabel, ui->formLayout);
    alert->SetFeedbackLabel(ui->emailFeedback, "Email");
    alert->SetFeedbackLabel(ui->titleFeedback, "Title");
    alert->SetFeedbackLabel(ui->descriptionFeedback, "Description");
    alert->SetFeedbackLabel(ui->attachmentFeedback, "Attachments");
    alert->SetFeedbackLabel(ui->summary, "Summary");
    PolulateListWithStaticItems();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetSearch(std::unique_ptr<ureport::SimilarCasesSearch> search)
{
    m_Search = std::move(search);
}

void MainWindow::SetFeedbackManager(std::unique_ptr<FeedbackManager> manager)
{
    m_FeedbackManager = std::move(manager);
    SetupFeedbackChecking();
    GetFeedback();
}

void MainWindow::SetupSearchProgressBar()
{
    m_ProgressIndicator = new QLabel(this);
    m_ProgressIndicator->setToolTip("Searching for similar cases...");
    QMovie* progressMovie = new QMovie(":/new/prefix1/progress.gif");
    m_ProgressIndicator->setMovie(progressMovie);
    m_ProgressIndicator->hide();
}

void MainWindow::AdjustSearchProgressBarPosition()
{
    AdjustWidgetsPosition(ui->titleEdit, m_ProgressIndicator, INDICATOR_AREA_SIZE);
}

void MainWindow::AdjustWidgetsPosition(QWidget* parentWidget, QWidget* widget, int size)
{
    const int x = parentWidget->pos().rx() + parentWidget->width()
        - size - 1;
    const int y = parentWidget->pos().ry() + parentWidget->height() / 2
        - size / 2;
    widget->resize(size, size);
    widget->move(x, y);
}

void MainWindow::ShowSearchProgressBar()
{
    AdjustSearchProgressBarPosition();

    m_ProgressIndicator->show();
    m_ProgressIndicator->movie()->start();
}

void MainWindow::HideSearchProgressBar()
{
    m_ProgressIndicator->hide();
    m_ProgressIndicator->movie()->stop();
}

void MainWindow::SetupProblemComboBoxItems()
{
    ui->problemComboBox->addItem("Please Specify",
        static_cast<int>(Report::kBugUnknown));
    ui->problemComboBox->addItem("A problem with the Editor",
        static_cast<int>(Report::kBugProblemWithEditor));
    ui->problemComboBox->addItem("A problem with the Player",
        static_cast<int>(Report::kBugProblemWithPlayer));
    ui->problemComboBox->addItem("Documentation",
        static_cast<int>(Report::kBugDocumentation));
    ui->problemComboBox->addItem("Crash bug",
        static_cast<int>(Report::kBugCrash));
    ui->problemComboBox->addItem("Services",
        static_cast<int>(Report::kBugServices));
}

void MainWindow::SetupReproComboBoxItems()
{
    ui->reproComboBox->addItem("Please Specify",
        static_cast<int>(Report::kReproUnknown));
    ui->reproComboBox->addItem("This is the first time",
        static_cast<int>(Report::kReproFirstTime));
    ui->reproComboBox->addItem("Sometimes but not always",
        static_cast<int>(Report::kReproSometimes));
    ui->reproComboBox->addItem("Always",
        static_cast<int>(Report::kReproAlways));
}

void MainWindow::SetupProgressDialog()
{
    m_ProgressDialog.setFont(font());
    m_ProgressDialog.setWindowModality(Qt::WindowModal);
    m_ProgressDialog.setMinimumDuration(500);
    m_ProgressDialog.setWindowTitle(windowTitle());
    auto flags = m_ProgressDialog.windowFlags();
    flags |= Qt::MSWindowsFixedSizeDialogHint;
    flags &= ~Qt::WindowContextHelpButtonHint;
    m_ProgressDialog.setWindowFlags(flags);
}

void MainWindow::SetupSearchPopup()
{
    m_SearchTimer = new QTimer(this);
    m_SearchTimer->setSingleShot(true);
    m_SearchTimer->setInterval(1000);


    connect(m_SearchTimer, SIGNAL(timeout()), SLOT(FindSimilarCases()));
    connect(ui->titleEdit, SIGNAL(textEdited(QString)), m_SearchTimer,
        SLOT(start()));
}

void MainWindow::SetupFeedbackChecking()
{
    m_FeedbackTimer = new QTimer(this);
    m_FeedbackTimer->setSingleShot(true);
    m_FeedbackTimer->setInterval(1000);

    connect(m_FeedbackTimer, SIGNAL(timeout()), SLOT(GetFeedback()));
    connect(ui->problemComboBox, SIGNAL(currentIndexChanged(int)), SLOT(GetFeedback()));
    connect(ui->reproComboBox, SIGNAL(currentIndexChanged(int)), SLOT(GetFeedback()));
    connect(ui->titleEdit, SIGNAL(textEdited(QString)), m_FeedbackTimer,
        SLOT(start()));
    connect(ui->emailEdit, SIGNAL(textEdited(QString)), m_FeedbackTimer,
        SLOT(start()));
    connect(ui->descriptionEdit, SIGNAL(textChanged()), m_FeedbackTimer,
        SLOT(start()));
    connect(&*m_AttachmentsModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)), m_FeedbackTimer, SLOT(start()));
}

void MainWindow::FindSimilarCases()
{
    ShowSearchProgressBar();

    auto title = ui->titleEdit->text().toStdString();

    m_Search->FindSimilarCases(title,
        [&] (std::shared_ptr<SearchResult> result) {ShowSearchResults(result); });

    AdjustSearchPopupPosition();
}

void MainWindow::GetFeedback()
{
    m_FeedbackManager->GiveFeedback(*m_Report, [&](std::map<std::string, Feedback> collection)
    {
        alert->RegenerateFromTasks(collection);
        m_BlockSendingByFeedback = m_FeedbackManager->ShouldSendBeBlocked();
        if (m_FeedbackManager->ShouldBugReporterBeDisabled())
            SetControlsEnabled(false);
        RefreshSendButton();
    });
}

void MainWindow::resizeEvent(QResizeEvent * ev)
{
    AdjustSearchPopupPosition();
}

void MainWindow::AdjustSearchPopupPosition()
{
    int resultsControlWidth = ui->searchResultsList->width();
    ui->searchResultsList->setColumnWidth(0, (int)(resultsControlWidth * 0.7));
    ui->searchResultsList->setColumnWidth(1, (int)(resultsControlWidth * 0.3));
}

void MainWindow::PopulateSearchResultsItems(
    std::shared_ptr<SearchResult> result)
{
    auto choices = result->GetResults();

    ui->searchResultsList->setUpdatesEnabled(false);
    ui->searchResultsList->clear();

    for (uint i = 0; i < choices.size(); ++i)
    {
        AddListItem(choices[i].m_Title.c_str(), choices[i].m_Url.c_str());
    }

    if (result->GetTotalResultsCount() > 0)
    {
        auto allResultsLink = QString("https://unity3d.com/search?gq=%1").arg(ui->titleEdit->text());
        AddListItem("Show all search results...", allResultsLink);
    }

    PolulateListWithStaticItems();
    ui->searchResultsList->setUpdatesEnabled(true);
}

void MainWindow::PolulateListWithStaticItems()
{
    AddListItem("How to report bugs", "http://unity3d.com/unity/qa/bug-reporting");
    AddListItem("Public issue tracker", "http://issuetracker.unity3d.com");
    AddListItem("Unity answers", "http://answers.unity3d.com");
    AddListItem("Unity forums", "http://forum.unity3d.com");
    AddListItem("Unity community", "https://unity3d.com/community");
    AddListItem("Privacy policy", "http://unity3d.com/legal/privacy-policy");
}

void MainWindow::AddListItem(QString title, QString url)
{
    auto item = new QTreeWidgetItem(ui->searchResultsList);
    QString labelText = QString("<a href='%1'>%2</a>").arg(url).arg(title);
    auto linkLabel = new QLabel(labelText);
    linkLabel->setContentsMargins(4, 1, 1, 1);
    linkLabel->setOpenExternalLinks(true);
    linkLabel->adjustSize();
    item->setText(1, QUrl(url).host());
    ui->searchResultsList->setItemWidget(item, 0, linkLabel);
}

void MainWindow::ShowSearchResults(std::shared_ptr<SearchResult> result)
{
    PopulateSearchResultsItems(result);

    HideSearchProgressBar();
}

void MainWindow::SetReportTitle(const QString& title)
{
    ui->titleEdit->setText(title);
}

QString MainWindow::GetReportTitle() const
{
    return ui->titleEdit->text();
}

void MainWindow::SetAdditionalInfo(const QString& text)
{
    ui->descriptionEdit->document()->setPlainText(text);
}

QString MainWindow::GetAdditionalInfo() const
{
    return ui->descriptionEdit->document()->toPlainText();
}

void MainWindow::SetReporterEmail(const QString& email)
{
    ui->emailEdit->setText(email);
}

QString MainWindow::GetReporterEmail() const
{
    return ui->emailEdit->text();
}

bool MainWindow::IsReportFilledUp() const
{
    return m_IsFilledUp;
}

void MainWindow::SetReport(std::shared_ptr<ureport::Report> report)
{
    m_Report = report;
    FixBugType();
    m_Report->AddMonitor(m_ReportChangedMonitor);
    UpdateView(*m_Report);
}

void MainWindow::FixBugType()
{
    m_AutoCrash = m_Report->GetBugTypeID() == ureport::Report::kBugCrashAuto;
    ui->problemComboBox->setEnabled(!m_AutoCrash);
}

void MainWindow::SetReportController(
    std::shared_ptr<ureport::ComposeReportController> controller)
{
    m_ReportController = controller;
}

void MainWindow::on_attachFileButton_clicked()
{
    const QStringList paths = QFileDialog::getOpenFileNames(
        this, tr("Attach Files"));
    std::for_each(paths.begin(), paths.end(), [this] (const QString& path)
    {
        auto const name = path.toStdString();
        if (!m_AttachmentsModel->HasAttachmentWithName(name))
        {
            try
            {
                m_ReportController->AttachFile(name);
            }
            catch (const std::exception& e)
            {
                ShowErrorMessage(e.what());
            }
        }
    });
    UpdateView(*m_Report);
}

void MainWindow::on_attachFolderButton_clicked()
{
    const QString path = QFileDialog::getExistingDirectory(
        this, tr("Attach Folder"), "", QFileDialog::ShowDirsOnly);
    auto const name = path.toStdString();
    if (!IsEmpty(name) && !m_AttachmentsModel->HasAttachmentWithName(name))
    {
        try
        {
            m_ReportController->AttachDirectory(path.toStdString());
        }
        catch (const std::exception& e)
        {
            ShowErrorMessage(e.what());
        }
    }
    UpdateView(*m_Report);
}

void MainWindow::on_removeFileButton_clicked()
{
    const auto selectedRows = ui->attachedFilesView->selectionModel()->
        selectedRows();
    std::vector<QModelIndex> modelRows;
    std::transform(selectedRows.begin(), selectedRows.end(),
        std::back_inserter(modelRows),
        [&] (const QModelIndex& index) {
            return m_AttachmentsProxyModel->mapToSource(index);
        });
    std::vector<std::shared_ptr<Attachment> > attachmentsToRemove;
    std::transform(modelRows.begin(), modelRows.end(),
        std::back_inserter(attachmentsToRemove),
        [&] (const QModelIndex& index) {
            return m_AttachmentsModel->GetAttachment(index);
        });
    m_AttachmentsModel->RemoveAttachments(modelRows);
    std::for_each(attachmentsToRemove.begin(), attachmentsToRemove.end(),
        [&] (const std::shared_ptr<Attachment> attachment) {
            m_ReportController->RemoveAttachment(attachment);
        });

    int rowToSelect = 0;
    if (selectedRows.count() > 0)
    {
        rowToSelect = selectedRows.last().row() - selectedRows.count() + 1;
        auto totalRows = ui->attachedFilesView->verticalHeader()->count();
        auto rowsLeftAfterDelete = totalRows - selectedRows.count();
        if (rowToSelect >= rowsLeftAfterDelete)
            rowToSelect = rowsLeftAfterDelete - 1;
    }
    UpdateView(*m_Report);
    ui->attachedFilesView->selectRow(rowToSelect);
}

void MainWindow::on_cancelButton_clicked()
{
    close();
}

void MainWindow::on_previewButton_clicked()
{
    m_ReportChangedMonitor->Block();
    m_ProgressDialog.setCancelButtonText(tr("Cancel"));
    m_ProgressDialog.setWindowTitle(tr("Preview"));
    m_PrepareReportThread.SetReportController(m_ReportController);
    m_PrepareReportThread.start();
}

void MainWindow::on_sendButton_clicked()
{
    m_ProgressDialog.setCancelButtonText(tr("Cancel"));
    m_ProgressDialog.setWindowTitle(tr("Uploading"));
    m_SendReportThread.SetReportController(m_ReportController);
    m_SendReportThread.start();
}

void MainWindow::on_saveButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Report To Disk"), "UnityBugReport.zip",
        tr("Unity Bug Report (*.zip);;All Files (*)"));
    if (fileName != "")
    {
        m_SaveReportThread.SetFileName(fileName.toStdString());
        m_ProgressDialog.setCancelButtonText(tr("Cancel"));
        m_ProgressDialog.setWindowTitle(tr("Saving"));
        m_SaveReportThread.SetReportController(m_ReportController);
        m_SaveReportThread.start();
    }
}

void MainWindow::OnTitleChanged(const QString& text)
{
    m_ReportController->SetTitle(text.toStdString());
}

void MainWindow::OnAdditionalInfoChanged()
{
    auto text = ui->descriptionEdit->document()->toPlainText().toStdString();
    m_ReportController->SetTextualDescription(text);
    UpdateDescriptionLength(int(text.length()));
}

void MainWindow::UpdateDescriptionLength(int descriptionLength)
{
    const int remainingChars = Report::DESCRIPTION_MAX_CHARS - descriptionLength;
    ui->charLimitLabel->setVisible(remainingChars <= WARNING_CHARS_LIMIT);
    ui->charLimitLabel->setText(QString::number(remainingChars));
    if (remainingChars < 0)
    {
        ui->charLimitLabel->setStyleSheet("QLabel { color : red; }");
        m_BlockSendingByDescription = true;
    }
    else
    {
        ui->charLimitLabel->setStyleSheet("");
        m_BlockSendingByDescription = false;
    }
    RefreshSendButton();
}

void MainWindow::OnEmailChanged(const QString& text)
{
    m_ReportController->SetReporterEmail(text.toStdString());
}

void MainWindow::OnBugTypeIDChanged(const int index)
{
    auto  type = static_cast<ureport::Report::BugTypeID>(
        ui->problemComboBox->itemData(index).toInt());
    m_ReportController->SetBugTypeID(type);
    m_DocumentationIssue = type == ureport::Report::kBugDocumentation;
    ui->reproComboBox->setEnabled(!m_DocumentationIssue);
    if (m_DocumentationIssue)
    {
        auto  alwaysReproductionType = static_cast<int>(
            ureport::Report::kReproAlways + 1);
        ui->reproComboBox->setCurrentIndex(alwaysReproductionType);
    }
}

void MainWindow::OnBugReproChanged(const int index)
{
    m_ReportController->SetBugReproducibility(
        static_cast<ureport::Report::BugReproducibility>(index - 1));
}

void MainWindow::UpdateView(const ureport::Report& report)
{
    ureport::ui::SignalBlocker blocker;
    blocker.Block(ui->titleEdit);
    blocker.Block(ui->descriptionEdit);
    blocker.Block(ui->emailEdit);
    blocker.Block(ui->problemComboBox);
    blocker.Block(ui->reproComboBox);
    ui->titleEdit->setText(QString::fromStdString(report.GetTitle()));
    auto const description = report.GetTextualDescription();
    ui->descriptionEdit->document()->setPlainText(
        QString::fromStdString(description));
    UpdateDescriptionLength(int(description.length()));
    ui->emailEdit->setText(QString::fromStdString(report.GetReporterEmail()));
    auto reportType = report.GetBugTypeID();
    if (reportType == ureport::Report::kBugCrashAuto)
    {
        reportType = ureport::Report::kBugCrash;
    }
    auto type = static_cast<int>(reportType);
    int itemIndex = ui->problemComboBox->findData(type);
    ui->problemComboBox->setCurrentIndex(itemIndex);
    ui->reproComboBox->setCurrentIndex(
        static_cast<int>(report.GetBugReproducibility()) + 1);
    UpdateAttachmentView(report);
}

std::vector<std::shared_ptr<Attachment> > GetRemovableAttachments(
    const std::list<std::shared_ptr<Attachment> >& src)
{
    std::vector<std::shared_ptr<Attachment> > removable;
    std::copy_if(src.begin(), src.end(), std::back_inserter(removable),
        [] (const std::shared_ptr<Attachment>& attachment) -> bool {
            const auto preview = attachment->GetTextPreview();
            return IsNull(preview) ? false : preview->IsRemovable();
        });
    return removable;
}

QString MainWindow::GetDefaultTextualDescription() const
{
    return tr(
        "1. What happened\n\n"
        "2. How we can reproduce it using the example you attached\n"
    );
}

void MainWindow::UpdateAttachmentView(const ureport::Report& report)
{
    ui->attachedFilesView->selectionModel()->clearSelection();
    m_AttachmentsModel->SetAttachments(report.GetAttachments());
    m_AttachmentsProxyModel->invalidate();
    ui->attachedFilesView->horizontalHeader()->setSectionHidden(2, true);
}

void MainWindow::OnPrepareStarted()
{
    m_ProgressDialog.reset();
    m_ProgressDialog.setValue(m_ProgressDialog.minimum());
    SetControlsEnabled(false);
}

void MainWindow::OnPrepareDone()
{
    UpdateView(*m_Report);
    m_ProgressDialog.close();
    PreviewDialog preview(this);
    preview.SetReport(m_Report);
    preview.SetAttachmentsModel(m_AttachmentsModel);
    preview.exec();
}

void MainWindow::OnPrepareFinished()
{
    SetControlsEnabled(true);
    m_ReportChangedMonitor->UnBlock();
}

void MainWindow::OnPrepareFailed(const QString& error)
{
    m_ProgressDialog.close();
    ShowErrorMessage(error);
}

void MainWindow::OnSendStarted()
{
    m_ProgressDialog.reset();
    m_ProgressDialog.setValue(m_ProgressDialog.minimum());
    SetControlsEnabled(false);
}

void MainWindow::OnReportStatusChanged(const QString& status)
{
    if (m_ProgressDialog.wasCanceled())
        return;
    m_ProgressDialog.setLabelText(status);
}

void MainWindow::OnReportProgressChanged(size_t current, size_t total)
{
    if (m_ProgressDialog.wasCanceled())
        return;
    m_ProgressDialog.setMaximum(static_cast<int>(total));
    m_ProgressDialog.setValue(static_cast<int>(current));
}

void MainWindow::OnSendDone()
{
    SetControlsEnabled(true);
    m_ReportChangedMonitor->Reset();
    m_ProgressDialog.close();
    ShowSuccessConfirmation();
    close();
}

void MainWindow::ShowSuccessConfirmation()
{
    QMessageBox infoBox(this);
    infoBox.setWindowTitle(tr("Thank you!"));
    infoBox.setText(tr("Your report has been submitted."
        " You will receive a confirmation email shortly."
        " If our engineering team has more information"
        " or questions about the problem, you'll be"
        " informed of that by email as well."));
    infoBox.setStandardButtons(QMessageBox::Ok);
    infoBox.setButtonText(QMessageBox::Ok, tr("OK"));
    infoBox.setFont(MainWindow::font());
    infoBox.setIconPixmap(GetIcon(":/new/prefix1/checkmark.png"));
    infoBox.exec();
}

void MainWindow::ShowSaveConfirmation()
{
    QMessageBox infoBox(this);
    infoBox.setWindowTitle(tr("Thank you!"));
    std::string message = std::string("Your report has been saved successfully to ")
        + m_SaveReportThread.GetFileName()
        + ". Please submit it to Unity Support via https://support.unity3d.com.";
    infoBox.setText(tr(message.c_str()));
    infoBox.setStandardButtons(QMessageBox::Ok);
    infoBox.setButtonText(QMessageBox::Ok, tr("OK"));
    infoBox.setFont(MainWindow::font());
    infoBox.setIconPixmap(GetIcon(":/new/prefix1/checkmark.png"));
    infoBox.exec();
}

void MainWindow::OnSendFailed(const QString& error)
{
    m_ProgressDialog.close();
    ShowErrorMessage(error);
}

void MainWindow::OnSendFinished()
{
    SetControlsEnabled(true);
}

void MainWindow::OnSaveStarted()
{
    m_ProgressDialog.reset();
    m_ProgressDialog.setValue(m_ProgressDialog.minimum());
    SetControlsEnabled(false);
}

void MainWindow::OnSaveDone()
{
    SetControlsEnabled(true);
    m_ReportChangedMonitor->Reset();
    m_ProgressDialog.close();
    ShowSaveConfirmation();
    close();
}

void MainWindow::OnSaveFailed(const QString& error)
{
    m_ProgressDialog.close();
    ShowErrorMessage(error);
}

void MainWindow::OnSaveFinished()
{
    SetControlsEnabled(true);
}

void MainWindow::SetControlsEnabled(bool state)
{
    ui->previewButton->setEnabled(state);
    m_BlockSendingByControls = !state;
    RefreshSendButton();
    ui->attachFolderButton->setEnabled(state);
    ui->attachFileButton->setEnabled(state);
    ui->problemComboBox->setEnabled(state && !m_AutoCrash);
    ui->reproComboBox->setEnabled(state && !m_DocumentationIssue);
    ui->emailEdit->setEnabled(state);
    ui->titleEdit->setEnabled(state);
    ui->descriptionEdit->setEnabled(state);
    ui->attachedFilesView->setEnabled(state);
}

void MainWindow::RefreshSendButton()
{
    if (m_BlockSendingByFeedback || m_BlockSendingByDescription || m_BlockSendingByControls)
    {
        ui->sendButton->setEnabled(false);
        ui->saveButton->setEnabled(false);
    }
    else
    {
        ui->saveButton->setEnabled(true);
        ui->sendButton->setEnabled(true);
    }
}

bool MainWindow::ConfirmExit()
{
    if (m_ReportChangedMonitor->IsReportChanged())
    {
        QMessageBox warningBox(this);
        warningBox.setWindowTitle(tr("Warning"));
        warningBox.setText(tr("Do you really want to quit?"
            " Your changes will be lost."));
        warningBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        warningBox.setButtonText(QMessageBox::Yes, tr("Yes"));
        warningBox.setButtonText(QMessageBox::No, tr("No"));
        warningBox.setFont(MainWindow::font());
        warningBox.setIconPixmap(GetIcon(":/new/prefix1/warning.png"));
        auto result = warningBox.exec();
        if (result != QMessageBox::Yes)
            return false;
    }
    return true;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (ui->cancelButton->isEnabled() && ConfirmExit())
    {
        SaveWindowState();
        event->accept();
    }
    else
        event->ignore();
}

void MainWindow::ShowErrorMessage(const QString& message)
{
    QMessageBox box(this);
    box.setWindowTitle(tr("Error"));
    box.setText(message);
    box.setFont(font());
    box.setIconPixmap(GetIcon(":/new/prefix1/error.png"));
    box.setButtonText(QMessageBox::Ok, tr("OK"));
    box.exec();
}

QPixmap MainWindow::GetIcon(const QString& resourceName) const
{
    QPixmap icon(resourceName);
    return icon.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void MainWindow::SaveWindowState() const
{
    auto settings = ureport::GetSettings();
    auto state = saveGeometry();
    ureport::WindowSettings windowSettings("mainwindow",
                                           QByteArrayToVector(state));
    settings->SetWindowState(windowSettings);
}

void MainWindow::RestoreWindowState()
{
    auto settings = ureport::GetSettings();
    auto savedState = settings->GetWindowState("mainwindow");
    auto state = VectorToQByteArray(savedState.m_WindowState);
    restoreGeometry(state);
}

void MainWindow::attachmentViewSelectionChanged(const QItemSelection& selected,
    const QItemSelection& deselected)
{
    ui->removeFileButton->setEnabled(
        ui->attachedFilesView->selectionModel()->hasSelection());
}
