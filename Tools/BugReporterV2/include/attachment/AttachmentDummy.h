#pragma once

#include "attachment/Attachment.h"
#include "file_system/FileDummy.h"
#include "reporter/FilesPacker.h"
#include "attachment_preview/AttachmentTextPreviewDummy.h"

namespace ureport
{
namespace test
{
    class AttachmentDummy : public ureport::Attachment
    {
        std::shared_ptr<File> Pack(FilesPacker& packer, LongTermOperObserver*)
        {
            return std::make_shared<FileDummy>();
        }

        std::shared_ptr<AttachmentTextPreview> GetTextPreview() const
        {
            return std::make_shared<AttachmentTextPreviewDummy>();
        }
    };
}
}
