#include "PackedDirAttachment.h"
#include "FlatFileAttachment.h"
#include "PackedMultipleFilesAttachment.h"

namespace ureport
{
    std::unique_ptr<Attachment> CreateFlatFileAttachment(
        std::shared_ptr<FileSystemEntry> entry,
        std::shared_ptr<FileGateway> gateway)
    {
        return std::unique_ptr<Attachment>(
            new details::FlatFileAttachment(entry, gateway));
    }

    std::unique_ptr<Attachment> CreatePackedMultipleFilesAttachment(
        std::list<std::shared_ptr<FileSystemEntry> > entries,
        std::shared_ptr<FileGateway> gateway,
        const std::string& filter,
        const std::string& name)
    {
        return std::unique_ptr<Attachment>(
            new details::PackedMultipleFilesAttachment(
                entries, gateway, filter, name));
    }
}
