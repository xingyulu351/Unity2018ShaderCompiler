#pragma once

#include "attachment/Attachment.h"
#include "FileDummy.h"
#include "reporter/FilesPacker.h"
#include "AttachmentTextPreviewDummy.h"

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

        std::string GetProperty(const std::string& name) const
        {
            return fileType;
        }

    public:
        std::string fileType;
    };
}
}
