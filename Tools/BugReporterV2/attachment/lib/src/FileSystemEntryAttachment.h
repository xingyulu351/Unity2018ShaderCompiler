#pragma once

#include "attachment/Attachment.h"
#include "attachment/AttachmentProperties.h"
#include "common/PathUtils.h"

#include <sstream>

namespace ureport
{
namespace details
{
    class FileSystemEntryAttachment : public ureport::Attachment
    {
    public:
    public:
        FileSystemEntryAttachment(std::shared_ptr<FileSystemEntry> entry,
                                  std::shared_ptr<FileGateway> gateway)
            : m_Entry(entry)
            , m_FileGateway(gateway)
        {
        }

        std::string GetProperty(const std::string& name) const override
        {
            if (name == attachment::kSize)
                return GetSizeProperty();
            else if (name == attachment::kType)
                return GetTypeProperty();
            else if (name == attachment::kName)
                return m_Entry->GetPath();
            return Attachment::GetProperty(name);
        }

    private:
        virtual std::string GetSizeProperty() const
        {
            std::stringstream property;
            property << m_Entry->GetSize();
            return property.str();
        }

        virtual std::string GetTypeProperty() const
        {
            return path_utils::GetSuffix(m_Entry->GetPath());
        }

    protected:
        std::shared_ptr<FileGateway> m_FileGateway;
        std::shared_ptr<FileSystemEntry> m_Entry;
    };
}
}
