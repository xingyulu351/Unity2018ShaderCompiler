#pragma once

#include "AttachmentItemView.h"

#include <QAbstractTableModel>
#include <memory>
#include <vector>
#include <list>

namespace ureport
{
    class Attachment;

namespace ui
{
    class AttachmentsTableModel : public QAbstractTableModel
    {
        Q_OBJECT

    public:
        enum Roles
        {
            kDisplayRole = Qt::DisplayRole,
            kUserRole = Qt::UserRole,
            kSortRole = kUserRole + 1,
            kFilterRole = kUserRole + 2,
        };

    public:
        AttachmentsTableModel(QObject* parent);

        void SetAttachments(const std::vector<std::shared_ptr<Attachment> >&
            attachments);

        void SetAttachments(const std::list<std::shared_ptr<Attachment> >&
            attachments);

        int columnCount(const QModelIndex& parent) const;

        QVariant headerData(int section, Qt::Orientation orientation,
            int role) const;

        int rowCount(const QModelIndex& parent) const;

        QVariant data(const QModelIndex& index, int role) const;

        const std::shared_ptr<Attachment> GetAttachment(size_t index) const;

        const std::shared_ptr<Attachment> GetAttachment(
            const QModelIndex& index) const;

        bool HasAttachmentWithName(const std::string& name) const;

        void RemoveAttachment(size_t index);

        void RemoveAttachment(const QModelIndex& index);

        void RemoveAttachments(const std::vector<size_t> indices);

        void RemoveAttachments(const std::vector<QModelIndex> indices);

    private:

        QVariant GetDisplayData(const QModelIndex& index) const;

        QVariant GetTooltip(const QModelIndex& index) const;

        QVariant GetSortData(const QModelIndex& index) const;

        QVariant GetFilterData(const QModelIndex& index) const;

        const AttachmentItemView& GetAttachmentView(size_t index) const;

        void CalculateViewData();

    private:
        std::vector<std::shared_ptr<AttachmentItemView> > m_AttachmentViews;
    };

    void SetAttachments(
        const std::vector<std::shared_ptr<Attachment> >& attachments);

    void SetAttachments(
        const std::list<std::shared_ptr<Attachment> >& attachments);

    inline
    const std::shared_ptr<Attachment> AttachmentsTableModel::GetAttachment(
        const QModelIndex& index) const
    {
        return GetAttachment(index.row());
    }

    inline
    void AttachmentsTableModel::RemoveAttachment(const QModelIndex& index)
    {
        RemoveAttachment(index.row());
    }
}
}
