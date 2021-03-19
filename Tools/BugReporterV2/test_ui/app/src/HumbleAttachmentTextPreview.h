#pragma once

#include "attachment_preview/AttachmentTextPreview.h"

#include <sstream>

namespace ureport
{
namespace test
{
    class HumbleAttachmentTextPreview : public ureport::AttachmentTextPreview
    {
        std::string GetName() const
        {
            std::stringstream name;
            name << "ATTACHMENT ";
            return name.str();
        }

        std::string GetDescription() const
        {
            std::stringstream descr;
            descr << "DESCRIPTION ";
            return descr.str();
        }

        std::vector<std::string> GetContent() const
        {
            std::stringstream line1;
            line1 << "LINE 1 ";
            std::stringstream line2;
            line2 << "LINE 2 ";
            std::vector<std::string> res;
            res.push_back(line1.str());
            res.push_back(line2.str());
            return res;
        }
    };
}
}
