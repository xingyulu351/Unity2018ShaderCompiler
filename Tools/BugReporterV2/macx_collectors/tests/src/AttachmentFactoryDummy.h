#pragma once

#include "attachment/AttachmentFactory.h"

namespace ureport
{
namespace test
{
    class AttachmentFactoryDummy : public ureport::AttachmentFactory
    {
        AttachmentPtr CreateFileAttachment(const std::string& path,
            const std::string& description) const override
        {
            return nullptr;
        }

        AttachmentPtr CreateDirectoryAttachment(const std::string& path,
            const std::string& description) const override
        {
            return nullptr;
        }

        AttachmentPtr CreateTextAttachment(const std::string& text,
            const std::string& description) const override
        {
            return nullptr;
        }

        AttachmentPtr CreateAttachment(const std::string& type,
            const std::map<std::string, std::string>& data) const override
        {
            return nullptr;
        }
    };
}
}
