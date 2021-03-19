#pragma once

#include "attachment/Attachment.h"
#include "HumbleAttachmentTextPreview.h"

#include <sstream>

namespace ureport
{
namespace test
{
    class HumbleAttachment : public ureport::Attachment
    {
        std::shared_ptr<AttachmentTextPreview> GetTextPreview() const override
        {
            return std::make_shared<HumbleAttachmentTextPreview>();
        }

        std::shared_ptr<File> Pack(FilesPacker& packer, LongTermOperObserver*) override
        {
            return std::shared_ptr<File>();
        }

        std::string GetProperty(const std::string& name) const override
        {
            return "";
        }
    };
}
}
