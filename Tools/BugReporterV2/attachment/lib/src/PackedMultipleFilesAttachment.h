#pragma once

#include "attachment/Attachment.h"
#include "PackedAttachmentsContainer.h"
#include "reporter/FilesPacker.h"
#include "file_system/FileGateway.h"
#include "file_system/FileSystemEntry.h"
#include "file_system/File.h"
#include "FileSystemAttachmentTextPreview.h"

namespace ureport
{
namespace details
{
    class PackedMultipleFilesAttachment : public Attachment
    {
    public:
        PackedMultipleFilesAttachment(
            std::list<std::shared_ptr<FileSystemEntry> > entries,
            std::shared_ptr<FileGateway> gateway, const std::string& filter,
            const std::string& name)
            : m_FSEntries(entries)
            , m_FileGateway(gateway)
            , m_Filter(filter)
            , m_Name(name)
        {
        }

    private:
        std::string GetProperty(const std::string& name) const
        {
            if (name == attachment::kName)
                return m_Name;
            return Attachment::GetProperty(name);
        }

        std::shared_ptr<AttachmentTextPreview> GetTextPreview() const
        {
            if (!m_Preview)
            {
                m_Preview.reset(new FileSystemAttachmentTextPreview(
                    [this] () { return m_Name; },
                    [this] () -> std::vector<std::string>
                    {
                        std::vector<std::string> paths;
                        ProcessEntries([&paths] (const std::string& path)
                            { paths.push_back(path); });
                        return paths;
                    }));
            }
            return m_Preview;
        }

        std::shared_ptr<File> Pack(
            FilesPacker& packer, LongTermOperObserver* observer) override
        {
            auto files = GetFiles();
            return files.empty() ?
                std::shared_ptr<File>() :
                std::make_shared<PackedAttachmentsContainer>(
                m_Name, packer.Pack(files, observer));
        }

        std::vector<std::shared_ptr<File> > GetFiles() const
        {
            std::vector<std::shared_ptr<File> > files;
            std::for_each(m_FSEntries.cbegin(), m_FSEntries.cend(),
                [&files, this] (const std::shared_ptr<FileSystemEntry> loc)
                {
                    GetLocationEntries(files, loc);
                });
            return files;
        }

        std::vector<std::shared_ptr<File> > GetFiles2() const
        {
            std::vector<std::shared_ptr<File> > files;
            ProcessEntries(
                [&files, this] (const std::string& path)
                {
                    files.push_back(m_FileGateway->GetFile(path));
                });
            return files;
        }

        void GetLocationEntries(std::vector<std::shared_ptr<File> >& files,
            const std::shared_ptr<FileSystemEntry> loc) const
        {
            const std::list<std::string>& entries = loc->GetEntries(m_Filter);
            std::for_each(entries.cbegin(), entries.cend(),
                [&files, this] (const std::string& path)
                {
                    files.push_back(m_FileGateway->GetFile(path));
                });
        }

        void ProcessEntries(
            std::function<void(const std::string&)> process) const
        {
            std::for_each(
                m_FSEntries.cbegin(),
                m_FSEntries.cend(),
                [this, &process] (std::shared_ptr<FileSystemEntry> entry)
                {
                    entry->ProcessEntries(m_Filter, process);
                });
        }

    private:
        std::list<std::shared_ptr<FileSystemEntry> > m_FSEntries;
        std::shared_ptr<FileGateway> m_FileGateway;
        std::string m_Filter;
        std::string m_Name;
        mutable std::shared_ptr<AttachmentTextPreview> m_Preview;
    };
}
}
