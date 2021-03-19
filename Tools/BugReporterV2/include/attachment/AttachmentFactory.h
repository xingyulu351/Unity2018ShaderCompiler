#pragma once

#include <memory>
#include <string>
#include <map>

namespace ureport
{
    class Attachment;
    class FileSystem;
    class FileGateway;

    class AttachmentFactory
    {
    public:
        typedef std::unique_ptr<Attachment> AttachmentPtr;

        virtual AttachmentPtr CreateFileAttachment(const std::string& path,
            const std::string& description) const = 0;

        virtual AttachmentPtr CreateDirectoryAttachment(const std::string& path,
            const std::string& description) const = 0;

        virtual AttachmentPtr CreateTextAttachment(const std::string& text,
            const std::string& description) const = 0;

        virtual AttachmentPtr CreateAttachment(const std::string& type,
            const std::map<std::string, std::string>& data) const = 0;

        virtual ~AttachmentFactory() {}
    };

    std::unique_ptr<AttachmentFactory> CreateAttachmentFactory(
        std::shared_ptr<FileSystem> fileSystem,
        std::shared_ptr<FileGateway> fileGateway);
}
