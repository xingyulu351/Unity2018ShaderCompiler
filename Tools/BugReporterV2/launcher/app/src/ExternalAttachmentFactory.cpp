#include "attachment/Attachment.h"
#include "attachment/AttachmentFactory.h"
#include "attachment_preview/AttachmentTextPreview.h"
#include "file_system/SystemFileGateway.h"
#include "file_system/FileSystem.h"

namespace ureport
{
namespace launcher
{
    class ExternalAttachmentFactory : public ureport::AttachmentFactory
    {
        std::unique_ptr<AttachmentFactory> m_Factory;

    public:
        ExternalAttachmentFactory(std::shared_ptr<FileSystem> fileSystem,
                                  std::shared_ptr<FileGateway> fileGateway)
            : m_Factory(CreateAttachmentFactory(fileSystem, fileGateway))
        {
        }

    private:
        AttachmentPtr CreateFileAttachment(const std::string& path,
            const std::string& description) const override
        {
            auto attachment = m_Factory->CreateFileAttachment(path, description);
            SetupAttachmentPreview(*attachment->GetTextPreview());
            return attachment;
        }

        AttachmentPtr CreateDirectoryAttachment(const std::string& path,
            const std::string& description) const override
        {
            auto attachment = m_Factory->CreateDirectoryAttachment(path, description);
            SetupAttachmentPreview(*attachment->GetTextPreview());
            return attachment;
        }

        AttachmentPtr CreateTextAttachment(const std::string& text,
            const std::string& description) const override
        {
            return m_Factory->CreateTextAttachment(text, description);
        }

        AttachmentPtr CreateAttachment(const std::string& type,
            const std::map<std::string, std::string>& data) const
        {
            return nullptr;
        }

        void SetupAttachmentPreview(AttachmentTextPreview& preview) const
        {
            preview.SetRemovable(true);
        }
    };

    std::unique_ptr<ureport::AttachmentFactory> CreateExternalAttachmentFactory()
    {
        return std::unique_ptr<ureport::AttachmentFactory>(
            new ExternalAttachmentFactory(CreateFileSystem(), CreateSystemFileGateway()));
    }
}
}
