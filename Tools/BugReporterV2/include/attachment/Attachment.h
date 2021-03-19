#pragma once

#include <memory>
#include <string>
#include <list>

namespace ureport
{
    class File;
    class FilesPacker;
    class FileSystemEntry;
    class FileGateway;
    class AttachmentTextPreview;
    class LongTermOperObserver;

    class Attachment
    {
    public:
        virtual std::shared_ptr<AttachmentTextPreview> GetTextPreview() const = 0;

        virtual std::shared_ptr<File> Pack(FilesPacker& packer, LongTermOperObserver* observer) = 0;

        virtual std::string GetProperty(const std::string& name) const
        {
            return std::string();
        }

        virtual ~Attachment() {}
    };

    std::unique_ptr<Attachment> CreateFlatFileAttachment(
        std::shared_ptr<FileSystemEntry> entry,
        std::shared_ptr<FileGateway> gateway);

    std::unique_ptr<Attachment> CreatePackedMultipleFilesAttachment(
        std::list<std::shared_ptr<FileSystemEntry> > entries,
        std::shared_ptr<FileGateway> gateway,
        const std::string& filter,
        const std::string& name);
}
