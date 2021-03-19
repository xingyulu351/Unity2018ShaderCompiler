#pragma once

#include "attachment_preview/AttachmentTextPreview.h"

namespace ureport
{
namespace test
{
    class AttachmentTextPreviewDummy : public ureport::AttachmentTextPreview
    {
    public:
        AttachmentTextPreviewDummy()
        {
            SetRemovable(true);
        }

        std::string GetName() const
        {
            return "fileName";
        }

        std::vector<std::string> GetContent() const
        {
            return std::vector<std::string>();
        }
    };
}
}
