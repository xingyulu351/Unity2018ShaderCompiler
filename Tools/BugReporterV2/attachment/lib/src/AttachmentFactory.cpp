#include "FlatFileAttachment.h"
#include "PackedDirAttachment.h"
#include "TextAttachment.h"
#include "attachment/Attachment.h"
#include "attachment/AttachmentFactory.h"
#include "attachment/AttachmentTypes.h"
#include "file_system/FileGateway.h"
#include "file_system/FileSystem.h"

#include <sstream>

namespace ureport
{
    class File;
    class FilesPacker;

namespace details
{
    class AttachmentFactoryImpl : public ureport::AttachmentFactory
    {
        std::shared_ptr<FileSystem> m_FileSystem;
        std::shared_ptr<FileGateway> m_FileGateway;

    public:
        AttachmentFactoryImpl(std::shared_ptr<FileSystem> fileSystem,
                              std::shared_ptr<FileGateway> fileGateway)
            : m_FileSystem(fileSystem)
            , m_FileGateway(fileGateway)
        {
        }

        AttachmentPtr CreateFileAttachment(const std::string& path,
            const std::string& description) const
        {
            auto entry = CreateFileSystemEntry(m_FileSystem, path);
            auto attachment = CreateFlatFileAttachment(entry, m_FileGateway);
            attachment->GetTextPreview()->SetDescription(description);
            return attachment;
        }

        AttachmentPtr CreateDirectoryAttachment(const std::string& path,
            const std::string& description) const
        {
            auto entry = CreateFileSystemEntry(m_FileSystem, path);
            auto attachment = AttachmentPtr(new details::PackedDirAttachment(
                entry, m_FileSystem, m_FileGateway, ""));
            attachment->GetTextPreview()->SetDescription(description);
            return attachment;
        }

        AttachmentPtr CreateTextAttachment(const std::string& text,
            const std::string& description) const
        {
            AttachmentPtr attachment(new TextAttachment(description, text));
            attachment->GetTextPreview()->SetDescription(description);
            return attachment;
        }

        AttachmentPtr CreateAttachment(const std::string& type,
            const std::map<std::string, std::string>& data) const
        {
            AttachmentPtr attachment = nullptr;
            if (type == attachment::kDirectoryType)
                attachment = CreateDirectoryAttachment(data.at("Name"),
                    data.at("Path"), data.at("Description"));
            if (type == attachment::kFileType)
                attachment = CreateFileAttachment(data.at("Path"),
                    data.at("Description"));
            if (attachment != nullptr)
                SetAttachmentProperties(*attachment, data);
            return attachment;
        }

        AttachmentPtr CreateDirectoryAttachment(const std::string& name,
            const std::string& path, const std::string& description) const
        {
            auto attachment(CreateDirectoryAttachment(path, description));
            PackedDirAttachment& dirAttachment =
                static_cast<PackedDirAttachment&>(*attachment);
            dirAttachment.SetPackageEntryName(name);
            return attachment;
        }

    private:
        void SetAttachmentProperties(Attachment& attachment,
            const std::map<std::string, std::string>& properties) const
        {
            auto p = properties.find("Removable");
            if (p != properties.end())
                attachment.GetTextPreview()->SetRemovable(p->second == "yes");
        }
    };
}

    std::unique_ptr<AttachmentFactory> CreateAttachmentFactory(
        std::shared_ptr<FileSystem> fileSystem,
        std::shared_ptr<FileGateway> fileGateway)
    {
        return std::unique_ptr<AttachmentFactory>(
            new details::AttachmentFactoryImpl(
                fileSystem, fileGateway));
    }
}
