#include "AttachmentsTableModel.h"
#include "AttachmentItemView.h"
#include "attachment/Attachment.h"
#include "attachment/AttachmentProperties.h"
#include "attachment_preview/AttachmentTextPreview.h"
#include "Phrases.h"

#include <algorithm>
#include <functional>

namespace ureport
{
namespace ui
{
    AttachmentsTableModel::AttachmentsTableModel(QObject* parent)
        : QAbstractTableModel(parent)
    {
    }

    bool HasAttachmentWithName(
        const std::vector<std::shared_ptr<AttachmentItemView> >& views,
        const std::string& name)
    {
        return std::any_of(views.cbegin(), views.cend(),
            [&name] (const std::shared_ptr<AttachmentItemView>& view) {
                return view->GetName() == name;
            });
    }

    template<typename Container>
    void UpdateAttachmentViews(
        std::vector<std::shared_ptr<AttachmentItemView> >& views,
        const Container& attachments)
    {
        std::vector<std::shared_ptr<AttachmentItemView> > viewsToAdd;
        std::for_each(attachments.cbegin(), attachments.cend(),
            [&views, &viewsToAdd] (const std::shared_ptr<Attachment>& attachment) {
                auto itemView = std::make_shared<AttachmentItemView>(attachment);
                if (!ui::HasAttachmentWithName(views, itemView->GetName()))
                {
                    viewsToAdd.push_back(itemView);
                }
            });
        views.insert(views.end(), viewsToAdd.begin(), viewsToAdd.end());
    }

    void AttachmentsTableModel::SetAttachments(
        const std::vector<std::shared_ptr<Attachment> >& attachments)
    {
        UpdateAttachmentViews(m_AttachmentViews, attachments);
        emit(dataChanged(QModelIndex(), QModelIndex()));
    }

    void AttachmentsTableModel::SetAttachments(
        const std::list<std::shared_ptr<Attachment> >& attachments)
    {
        UpdateAttachmentViews(m_AttachmentViews, attachments);
        emit(dataChanged(QModelIndex(), QModelIndex()));
    }

    int AttachmentsTableModel::columnCount(const QModelIndex& parent) const
    {
        return 3;
    }

    QVariant AttachmentsTableModel::headerData(int section,
        Qt::Orientation orientation, int role) const
    {
        switch (role)
        {
            case Qt::DisplayRole:
                switch (section)
                {
                    case 0:
                        return tr("Path");
                    case 1:
                        return tr("Type");
                    case 2:
                        return tr("Size");
                }
        }
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    int AttachmentsTableModel::rowCount(const QModelIndex& parent) const
    {
        return static_cast<int>(m_AttachmentViews.size());
    }

    QVariant AttachmentsTableModel::data(const QModelIndex & index,
        int role) const
    {
        if (role == kDisplayRole)
            return GetDisplayData(index);
        else if (role == Qt::ToolTipRole)
            return GetTooltip(index);
        else if (role == kSortRole)
            return GetSortData(index);
        else if (role == kFilterRole)
            return GetFilterData(index);
        return QVariant();
    }

    QVariant AttachmentsTableModel::GetDisplayData(
        const QModelIndex & index) const
    {
        const auto& view = GetAttachmentView(index.row());
        switch (index.column())
        {
            case 0:
                return QString::fromStdString(view.GetName());
            case 1:
                return Phrases::Translate(view.GetType());
            case 2:
                return QString::fromStdString(view.GetSize());
        }
        return QVariant();
    }

    QVariant AttachmentsTableModel::GetTooltip(const QModelIndex & index) const
    {
        const auto attachment = GetAttachment(index.row());
        const auto preview = attachment->GetTextPreview();
        return QString::fromStdString(preview->GetName());
    }

    QVariant AttachmentsTableModel::GetSortData(const QModelIndex& index) const
    {
        const auto& view = GetAttachmentView(index.row());
        switch (index.column())
        {
            case 0:
                return QString::fromStdString(view.GetName());
            case 1:
                return Phrases::Translate(view.GetType());
            case 2:
                return QVariant::fromValue(view.GetSizeInBytes());
        }
        return QVariant();
    }

    QVariant AttachmentsTableModel::GetFilterData(
        const QModelIndex& index) const
    {
        const auto attachment = GetAttachment(index.row());
        const auto preview = attachment->GetTextPreview();
        return preview->IsRemovable()
            ? QVariant::fromValue(tr("removable"))
            : QVariant();
    }

    const std::shared_ptr<Attachment> AttachmentsTableModel::GetAttachment(
        size_t index) const
    {
        const auto& view = GetAttachmentView(index);
        return view.GetAttachment();
    }

    const AttachmentItemView& AttachmentsTableModel::GetAttachmentView(
        size_t index) const
    {
        return *m_AttachmentViews.at(index);
    }

    bool AttachmentsTableModel::HasAttachmentWithName(
        const std::string& name) const
    {
        return ui::HasAttachmentWithName(m_AttachmentViews, name);
    }

    void AttachmentsTableModel::RemoveAttachment(size_t index)
    {
        m_AttachmentViews.erase(m_AttachmentViews.begin() + index);
        emit(dataChanged(QModelIndex(), QModelIndex()));
    }

    void AttachmentsTableModel::RemoveAttachments(
        const std::vector<size_t> indices)
    {
        std::for_each(indices.begin(), indices.end(),
            [&] (size_t index) {
                m_AttachmentViews[index] = std::shared_ptr<AttachmentItemView>();
            });
        auto removed = std::remove_if(m_AttachmentViews.begin(),
            m_AttachmentViews.end(),
            std::bind2nd(std::equal_to<std::shared_ptr<AttachmentItemView> >(),
                nullptr));
        m_AttachmentViews.erase(removed, m_AttachmentViews.end());
        emit(dataChanged(QModelIndex(), QModelIndex()));
    }

    void AttachmentsTableModel::RemoveAttachments(
        const std::vector<QModelIndex> indices)
    {
        std::vector<size_t> rows;
        std::transform(indices.begin(), indices.end(),
            std::back_inserter(rows), std::mem_fun_ref(&QModelIndex::row));
        RemoveAttachments(rows);
    }

    void AttachmentsTableModel::CalculateViewData()
    {
        std::for_each(m_AttachmentViews.begin(), m_AttachmentViews.end(),
            [] (const std::shared_ptr<AttachmentItemView>& view) {
                view->GetSize();
            });
    }
}
}
