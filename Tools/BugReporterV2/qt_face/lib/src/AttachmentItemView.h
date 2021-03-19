#pragma once

#include "shims/logical/IsEmpty.h"

#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

namespace ureport
{
    class Attachment;

namespace ui
{
    class AttachmentItemView
    {
    public:
        template<typename Container>
        static std::vector<std::shared_ptr<AttachmentItemView> > MakeItems(
            const Container& attachments)
        {
            std::vector<std::shared_ptr<AttachmentItemView> > items;
            std::transform(attachments.begin(), attachments.end(),
                std::back_inserter(items),
                [] (const std::shared_ptr<Attachment>& attachment)
                {
                    return std::make_shared<AttachmentItemView>(attachment);
                });
            return items;
        }

        AttachmentItemView(std::shared_ptr<Attachment> attachment);

        std::string GetName() const;

        std::string GetSize() const;

        size_t GetSizeInBytes() const;

        std::string GetType() const;

        std::shared_ptr<Attachment> GetAttachment() const;

    private:
        std::string GetSizeProperty() const;

        std::string SizeValueAsString(float value, int precision) const;

    private:
        const int kSizePrecision;
        std::shared_ptr<Attachment> m_Attachment;
        mutable std::string m_CachedSize;
    };
}
}
