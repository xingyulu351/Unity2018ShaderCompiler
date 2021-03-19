#pragma once

#include "AttachmentFactoryDummy.h"
#include "attachment/Attachment.h"
#include "attachment_preview/AttachmentTextPreview.h"

#include <memory>
#include <vector>

namespace ureport
{
namespace test
{
    class NullAttachmentPreview : public ureport::AttachmentTextPreview
    {
        std::string GetName() const override
        {
            return std::string();
        }

        std::vector<std::string> GetContent() const override
        {
            return std::vector<std::string>();
        }
    };

    class NullAttachment : public ureport::Attachment
    {
        std::shared_ptr<AttachmentTextPreview> GetTextPreview() const override
        {
            return std::make_shared<NullAttachmentPreview>();
        }

        std::shared_ptr<File> Pack(FilesPacker& packer, LongTermOperObserver* observer) override
        {
            return nullptr;
        }
    };

    class AttachmentFactorySpy : public AttachmentFactoryDummy
    {
    public:
        mutable std::vector<std::pair<std::string, std::string> > m_Files;
        mutable std::vector<std::pair<std::string, std::string> > m_Texts;

        AttachmentPtr CreateFileAttachment(const std::string& path,
            const std::string& description) const override
        {
            m_Files.push_back(std::make_pair(path, description));
            return std::unique_ptr<Attachment>(new NullAttachment());
        }

        AttachmentPtr CreateTextAttachment(const std::string& text,
            const std::string& description) const override
        {
            m_Texts.push_back(std::make_pair(text, description));
            return std::unique_ptr<Attachment>(new NullAttachment());
        }

        AttachmentPtr CreateAttachment(const std::string& type,
            const std::map<std::string, std::string>& data) const override
        {
            return CreateFileAttachment(data.at("Path"), data.at("Description"));
        }
    };
}
}
