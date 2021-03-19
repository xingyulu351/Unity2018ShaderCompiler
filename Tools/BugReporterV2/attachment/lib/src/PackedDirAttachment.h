#pragma once

#include "attachment/Attachment.h"
#include "PackedAttachmentsContainer.h"
#include "reporter/FilesPacker.h"
#include "file_system/FileGateway.h"
#include "file_system/FileSystem.h"
#include "file_system/FileSystemEntry.h"
#include "file_system/File.h"
#include "file_system/FileWithPath.h"
#include "common/PathUtils.h"
#include "FileSystemEntryAttachment.h"
#include "FileSystemAttachmentTextPreview.h"

namespace ureport
{
namespace details
{
    class PackedDirAttachment : public FileSystemEntryAttachment
    {
    public:
        PackedDirAttachment(std::shared_ptr<FileSystemEntry> entry,
                            std::shared_ptr<FileSystem> fileSystem,
                            std::shared_ptr<FileGateway> gateway, const std::string& filter)
            : FileSystemEntryAttachment(entry, gateway)
            , m_FileSystem(fileSystem)
            , m_Filter(filter)
        {
        }

    public:
        void SetPackageEntryName(const std::string& name)
        {
            m_PackageEntryName = name;
        }

        std::string GetPackageEntryName() const
        {
            return !IsEmpty(m_PackageEntryName)
                ? m_PackageEntryName
                : path_utils::GetBaseName(m_Entry->GetPath());
        }

    private:
        std::shared_ptr<AttachmentTextPreview> GetTextPreview() const override
        {
            if (!m_Preview)
            {
                m_Preview.reset(new FileSystemAttachmentTextPreview(
                    [this] () { return m_Entry->GetPath(); },
                    [this] () -> std::vector<std::string>
                    {
                        std::vector<std::string> paths;
                        m_Entry->ProcessEntries(m_Filter, [&paths] (const std::string& path)
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
            return files.empty() ? std::shared_ptr<File>() :
                std::make_shared<PackedAttachmentsContainer>(
                GetPackageEntryName(), packer.Pack(files, observer));
        }

        std::vector<std::shared_ptr<File> > GetFiles() const
        {
            std::vector<std::shared_ptr<File> > files;
            const std::list<std::string>& entries = m_Entry->GetEntries(m_Filter);
            std::for_each(entries.cbegin(), entries.cend(),
                [this, &files] (const std::string& filePath) {
                    files.push_back(CreateFile(filePath));
                });
            return files;
        }

        std::unique_ptr<File> CreateFile(const std::string& path) const
        {
            return m_FileSystem->IsDir(path)
                ? CreateFileForDirectory(path)
                : CreateFileForFile(path);
        }

        std::unique_ptr<File> CreateFileForFile(const std::string& path) const
        {
            auto dirPath = m_Entry->GetPath();
            return std::unique_ptr<File>(new FileWithPath(
                m_FileGateway->GetFile(path),
                [dirPath] (const std::string& path) -> std::string {
                    return path_utils::GetRelativePath(path, dirPath);
                }));
        }

        std::unique_ptr<File> CreateFileForDirectory(const std::string& path) const
        {
            auto dirPath = m_Entry->GetPath();
            return std::unique_ptr<File>(new FileWithPath(
                m_FileGateway->GetFile(path),
                [dirPath] (const std::string& path) -> std::string {
                    return path_utils::GetRelativePath(path + '/', dirPath);
                }));
        }

        std::string GetTypeProperty() const override
        {
            return "folder";
        }

    private:
        std::shared_ptr<FileSystem> m_FileSystem;
        std::string m_PackageEntryName;
        std::string m_Filter;
        mutable std::shared_ptr<AttachmentTextPreview> m_Preview;
    };
}
}
