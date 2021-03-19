#pragma once

#include "file_system/File.h"
#include "file_system/FileSystemEntry.h"
#include "file_system/FileGateway.h"
#include "FileSystemAttachmentTextPreview.h"
#include "file_system/File.h"
#include "common/PathUtils.h"
#include "common/StringUtils.h"
#include "system_interplay/FileSystem.h"
#include "FileSystemEntryAttachment.h"

#include <fstream>
#include <memory>
#include <string>

namespace ureport
{
namespace details
{
    class FileThatResidesInTopLevelOfPackage : public File
    {
    public:
        FileThatResidesInTopLevelOfPackage(const std::string& path, std::unique_ptr<File> file)
            : m_Name(path_utils::GetBaseName(path))
            , m_File(std::move(file))
        {
        }

    private:
        std::string GetPath() const
        {
            return m_Name;
        }

        bool IsDir() const override
        {
            return m_File->IsDir();
        }

        std::unique_ptr<std::istream> Read() const
        {
            return m_File->Read();
        }

        void Write(std::istream& data)
        {
        }

    private:
        std::string m_Name;
        std::shared_ptr<File> m_File;
    };

    std::vector<std::string> GetShortPreview(std::istream& stream, size_t width, size_t length);

    class FlatFileAttachment : public FileSystemEntryAttachment
    {
    public:
        FlatFileAttachment(std::shared_ptr<FileSystemEntry> entry,
                           std::shared_ptr<FileGateway> gateway)
            : FileSystemEntryAttachment(entry, gateway)
        {
        }

    private:
        std::shared_ptr<File> Pack(FilesPacker&, LongTermOperObserver*) override
        {
            auto file(m_FileGateway->GetFile(m_Entry->GetPath()));
            return std::make_shared<FileThatResidesInTopLevelOfPackage>(
                m_Entry->GetPath(), std::move(file));
        }

        std::shared_ptr<AttachmentTextPreview> GetTextPreview() const override
        {
            if (!m_Preview)
            {
                m_Preview.reset(new FileSystemAttachmentTextPreview(
                    [this] () { return m_Entry->GetPath(); },
                    [this] () { return GetFileContentPreview(); }));
            }
            return m_Preview;
        }

        std::vector<std::string> GetFileContentPreview() const
        {
            const int LINES_COUNT = 50;
            const int LINE_WIDTH = 1000;

            auto f = OpenFileStream(m_Entry->GetPath());

            return GetShortPreview(*f, LINE_WIDTH, LINES_COUNT);
        }

    private:
        mutable std::shared_ptr<AttachmentTextPreview> m_Preview;
    };
}
}
