#include "PreviewDialog.h"
#include "ui_PreviewDialog.h"
#include "AttachmentsTableModel.h"
#include "SignalBlocker.h"
#include "reporter/Report.h"
#include "AttachmentItemView.h"
#include "attachment/Attachment.h"
#include "attachment_preview/AttachmentTextPreview.h"
#include "settings/Settings.h"
#include "common/CustomQtUtils.h"

#include <QCloseEvent>
#include <QVariant>
#include <sstream>

Q_DECLARE_METATYPE(std::shared_ptr<ureport::Attachment>)

using namespace ::ureport;
using namespace ::ureport::ui;

PreviewDialog::PreviewDialog(
    QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PreviewDialog)
    , m_AttachmentsProxyModel(new QSortFilterProxyModel(this))
{
    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags);
    ui->setupUi(this);
    RestoreWindowState();
}

PreviewDialog::~PreviewDialog()
{
    delete ui;
}

void PreviewDialog::on_closeButton_clicked()
{
    close();
}

void PreviewDialog::SetReport(std::shared_ptr<ureport::Report> report)
{
    m_Report = report;
    UpdateView(*m_Report);
}

void PreviewDialog::SetAttachmentsModel(
    std::shared_ptr<ureport::ui::AttachmentsTableModel> model)
{
    m_AttachmentsModel = model;
    m_AttachmentsProxyModel->setSourceModel(m_AttachmentsModel.get());
    m_AttachmentsProxyModel->setSortRole(AttachmentsTableModel::kSortRole);
    ui->attachedFilesView->setModel(m_AttachmentsProxyModel.get());
    ui->attachedFilesView->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft);
    ui->attachedFilesView->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::Stretch);
    ui->attachedFilesView->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::Fixed);
    ui->attachedFilesView->horizontalHeader()->setSectionResizeMode(
        2, QHeaderView::Fixed);
    ui->attachedFilesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->attachedFilesView->setGridStyle(Qt::SolidLine);
    ui->attachedFilesView->setSortingEnabled(true);
    connect(ui->attachedFilesView, SIGNAL(activated(const QModelIndex&)),
        SLOT(on_attachedFilesView_activated(const QModelIndex&)));
}

void PreviewDialog::UpdateAttachments(const ureport::Report& report,
    ureport::ui::SignalBlocker& blocker)
{
    m_AttachmentsProxyModel->invalidate();
    ui->attachedFilesView->update();
}

QString PreviewDialog::GetBugTypeIDText(const ureport::Report::BugTypeID id)
{
    switch (id)
    {
        case ureport::Report::kBugProblemWithEditor:
            return tr("Related to: A problem with the Editor");
        case ureport::Report::kBugProblemWithPlayer:
            return tr("Related to: A problem with the Player");
        case ureport::Report::kBugFeatureRequest:
            return tr("Related to: A new feature request");
        case ureport::Report::kBugDocumentation:
            return tr("Related to: A problem with the documentation");
        case ureport::Report::kBugCrash:
            return tr("Related to: Crash");
        case ureport::Report::kBugCrashAuto:
            return tr("Related to: Crash");
        case ureport::Report::kBugServices:
            return tr("Related to: Services");
        default:
            return tr("Related to: Unknown");
    }
}

QString PreviewDialog::GetBugReproducibilityText(
    const ureport::Report::BugReproducibility repro)
{
    switch (repro)
    {
        case ureport::Report::kReproAlways:
            return tr("Reproducibility: Always");
        case ureport::Report::kReproSometimes:
            return tr("Reproducibility: Sometimes");
        case ureport::Report::kReproFirstTime:
            return tr("Reproducibility: First time");
        default:
            return tr("Reproducibility: Unknown");
    }
}

void PreviewDialog::UpdateDescription(const ureport::Report& report,
    ureport::ui::SignalBlocker& blocker)
{
    blocker.Block(ui->descriptionText);
    ui->descriptionText->clear();
    ui->descriptionText->setFontUnderline(true);
    ui->descriptionText->append(GetBugTypeIDText(report.GetBugTypeID()));
    ui->descriptionText->append(GetBugReproducibilityText(
        report.GetBugReproducibility()));
    ui->descriptionText->append(QString::fromStdString(
        report.GetReporterEmail()));
    ui->descriptionText->setFontUnderline(false);
    ui->descriptionText->setFontWeight(QFont::Bold);
    ui->descriptionText->insertPlainText('\n' +
        QString::fromStdString(report.GetTitle()) + '\n');
    ui->descriptionText->setFontWeight(QFont::Normal);
    ui->descriptionText->insertPlainText('\n' + QString::fromStdString(
        report.GetTextualDescription()));
    auto cursor = ui->descriptionText->textCursor();
    cursor.setPosition(0);
    ui->descriptionText->setTextCursor(cursor);
}

void PreviewDialog::UpdateView(const ureport::Report& report)
{
    ureport::ui::SignalBlocker blocker;
    UpdateAttachments(report, blocker);
    UpdateDescription(report, blocker);
}

QString ComposeContent(const std::vector<std::string>& content)
{
    std::stringstream ss;
    std::for_each(
        content.cbegin(), content.cend(),
        [&ss] (const std::string& line)
        {
            ss << line << std::endl;
        });
    return QString::fromStdString(ss.str());
}

void PreviewDialog::on_attachedFilesView_activated(const QModelIndex& index)
{
    ureport::ui::SignalBlocker blocker;
    blocker.Block(ui->attachmentDescriptionLabel);
    blocker.Block(ui->attachmentDetailsText);
    auto attachments(m_Report->GetAttachments());
    if (index.isValid())
    {
        const auto attachment = m_AttachmentsModel->GetAttachment(
            m_AttachmentsProxyModel->mapToSource(index));
        auto preview(attachment->GetTextPreview());
        QString descr(tr("Description: "));
        descr += QString::fromStdString(preview->GetDescription());
        ui->attachmentDescriptionLabel->setText(descr);
        ui->attachmentDetailsText->setPlainText(ComposeContent(
            preview->GetContent()));
    }
}

void PreviewDialog::closeEvent(QCloseEvent* event)
{
    SaveWindowState();
}

void PreviewDialog::SaveWindowState() const
{
    auto settings = ureport::GetSettings();
    auto state = saveGeometry();
    ureport::WindowSettings windowSettings("previewdialog", QByteArrayToVector(state));
    settings->SetWindowState(windowSettings);
}

void PreviewDialog::RestoreWindowState()
{
    auto settings = ureport::GetSettings();
    auto savedState = settings->GetWindowState("previewdialog");
    auto state = VectorToQByteArray(savedState.m_WindowState);
    restoreGeometry(state);
}
