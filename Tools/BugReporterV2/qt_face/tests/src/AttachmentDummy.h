#pragma once

#include "attachment/Attachment.h"
#include "attachment_preview/AttachmentTextPreview.h"
#include "file_system/File.h"

namespace ureport
{
namespace test
{
    class AttachmentDummy : public ureport::Attachment
    {
    public:
        std::shared_ptr<AttachmentTextPreview> GetTextPreview() const override
        {
            return nullptr;
        }

        std::shared_ptr<File> Pack(FilesPacker& packer, LongTermOperObserver* observer) override
        {
            return nullptr;
        }

        std::string GetProperty(const std::string& name) const override
        {
            return std::string();
        }
    };
}
}
