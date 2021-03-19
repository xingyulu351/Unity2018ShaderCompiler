#pragma once

#include "attachment/Attachment.h"

namespace ureport
{
    class TextAttachment : public ureport::Attachment
    {
        std::shared_ptr<AttachmentTextPreview> m_Preview;
        std::shared_ptr<File> m_File;

    public:
        TextAttachment(const std::string& name, const std::string& text);

        std::string GetProperty(const std::string& name) const override;

        std::shared_ptr<AttachmentTextPreview> GetTextPreview() const override;

        std::shared_ptr<File> Pack(FilesPacker& packer, LongTermOperObserver* observer) override;
    };
}
