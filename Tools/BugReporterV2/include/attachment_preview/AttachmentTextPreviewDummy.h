#pragma once

#include "attachment_preview/AttachmentTextPreview.h"

namespace ureport
{
namespace test
{
    class AttachmentTextPreviewDummy : public ureport::AttachmentTextPreview
    {
        std::string GetName() const
        {
            return "";
        }

        std::string GetDescription() const
        {
            return "";
        }

        std::vector<std::string> GetContent() const
        {
            return std::vector<std::string>();
        }
    };
}
}
