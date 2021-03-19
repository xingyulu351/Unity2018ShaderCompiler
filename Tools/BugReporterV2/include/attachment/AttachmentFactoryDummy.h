#pragma once

#include "attachment/AttachmentFactory.h"
#include "AttachmentDummy.h"

#include <memory>
#include <utility>

namespace ureport
{
namespace test
{
    class AttachmentFactoryDummy : public ureport::AttachmentFactory
    {
        AttachmentPtr CreateFileAttachment(const std::string& path,
            const std::string& description) const override
        {
            return std::unique_ptr<Attachment>(new AttachmentDummy());
        }

        AttachmentPtr CreateDirectoryAttachment(const std::string& path,
            const std::string& description) const override
        {
            return std::unique_ptr<Attachment>(new AttachmentDummy());
        }

        AttachmentPtr CreateTextAttachment(const std::string& text,
            const std::string& description) const override
        {
            return std::unique_ptr<Attachment>(new AttachmentDummy());
        }

        AttachmentPtr CreateAttachment(const std::string& type,
            const std::map<std::string, std::string>& data) const
        {
            return nullptr;
        }
    };
}
}
