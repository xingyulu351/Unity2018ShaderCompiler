#pragma once

#include "reporter/Report.h"

#include <QDialog>
#include <QSortFilterProxyModel>
#include <memory>

namespace Ui
{
    class PreviewDialog;
}

namespace ureport
{
    class Report;

namespace ui
{
    class SignalBlocker;
    class AttachmentsTableModel;
}
}

class PreviewDialog : public QDialog
{
    Ui::PreviewDialog* ui;
    std::shared_ptr<ureport::Report> m_Report;
    std::shared_ptr<ureport::ui::AttachmentsTableModel> m_AttachmentsModel;
    std::shared_ptr<QSortFilterProxyModel> m_AttachmentsProxyModel;

    Q_OBJECT

public:

    explicit PreviewDialog(QWidget *parent = 0);

    ~PreviewDialog();

    void SetReport(std::shared_ptr<ureport::Report> report);

    void SetAttachmentsModel(
        std::shared_ptr<ureport::ui::AttachmentsTableModel> model);

private:
    void UpdateView(const ureport::Report& report);

    void UpdateAttachments(const ureport::Report& report,
        ureport::ui::SignalBlocker& blocker);

    void UpdateDescription(const ureport::Report& report,
        ureport::ui::SignalBlocker& blocker);

    QString GetBugTypeIDText(const ureport::Report::BugTypeID id);

    QString GetBugReproducibilityText(
        const ureport::Report::BugReproducibility repro);

    void closeEvent(QCloseEvent* event);

    void SaveWindowState() const;

    void RestoreWindowState();

private slots:

    void on_closeButton_clicked();

    void on_attachedFilesView_activated(const QModelIndex& index);
};
