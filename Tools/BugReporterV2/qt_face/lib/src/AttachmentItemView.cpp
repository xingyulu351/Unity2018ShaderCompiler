#include "AttachmentItemView.h"
#include "attachment/Attachment.h"
#include "attachment/AttachmentProperties.h"
#include "Phrases.h"

#include <stdexcept>
#include <exception>
#include <sstream>
#include <QDir>

namespace ureport
{
namespace ui
{
    AttachmentItemView::AttachmentItemView(
        std::shared_ptr<Attachment> attachment)
        : m_Attachment(attachment)
        , kSizePrecision(1)
    {
        if (m_Attachment == nullptr)
            throw std::invalid_argument("Attachment can not be null");
    }

    std::string AttachmentItemView::GetName() const
    {
        auto name = m_Attachment->GetProperty(attachment::kName);
        return QDir::toNativeSeparators(QString::fromStdString(name)).toStdString();
    }

    std::string AttachmentItemView::GetSizeProperty() const
    {
        if (IsEmpty(m_CachedSize))
        {
            m_CachedSize = m_Attachment->GetProperty(attachment::kSize);
        }
        return m_CachedSize;
    }

    size_t AttachmentItemView::GetSizeInBytes() const
    {
        const auto sizeProperty = GetSizeProperty();
        if (IsEmpty(sizeProperty))
            return 0;
        if (sizeProperty == "?")
            return 0;
        size_t exactSize;
        std::istringstream(sizeProperty) >> exactSize;
        return exactSize;
    }

    std::string AttachmentItemView::GetSize() const
    {
        const auto exactSize = GetSizeProperty();
        if (IsEmpty(exactSize))
            return "";
        if (exactSize == "?")
            return "?";
        const unsigned long long size = std::stoll(exactSize);
        std::stringstream result;
        const size_t kKilo = 1024;
        const size_t kMega = kKilo * kKilo;
        const size_t kGiga = kMega * kKilo;
        if (size < kKilo)
            result << size << " " << Phrases::Translate("bytes").toStdString();
        else if (size < kMega)
            result << size / kKilo << " " << Phrases::Translate("Kb").toStdString();
        else if (size < kGiga)
            result << SizeValueAsString(float(size) / kMega, kSizePrecision)
            << " " << Phrases::Translate("Mb").toStdString();
        else
            result << SizeValueAsString(float(size) / kGiga, kSizePrecision)
            << " " << Phrases::Translate("Gb").toStdString();
        return result.str();
    }

    std::string AttachmentItemView::SizeValueAsString(float value,
        int precision) const
    {
        std::stringstream tmp;
        tmp.precision(precision);
        tmp << std::fixed << value;
        const auto valueStr = tmp.str();
        auto lastNonZeroPos = valueStr.find_last_not_of('0');
        if (lastNonZeroPos == valueStr.size() - 1)
            return valueStr;
        else if (lastNonZeroPos == valueStr.find_first_of('.'))
            return valueStr.substr(0, lastNonZeroPos);
        else
            return valueStr.substr(0, lastNonZeroPos + 1);
    }

    std::string AttachmentItemView::GetType() const
    {
        return m_Attachment->GetProperty(attachment::kType);
    }

    std::shared_ptr<Attachment> AttachmentItemView::GetAttachment() const
    {
        return m_Attachment;
    }
}
}
