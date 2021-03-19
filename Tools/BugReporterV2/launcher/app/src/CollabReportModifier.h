#pragma once

#include "attachment/Attachment.h"
#include "attachment/AttachmentFactory.h"
#include "common/PathUtils.h"
#include "file_system/FileSystem.h"
#include "file_system/SystemFileGateway.h"
#include "reporter/ReportDraft.h"
#include "reporter/ReportModifier.h"
#include "shims/logical/IsEmpty.h"

#include <regex>

namespace ureport
{
    class CollabReportModifier : public ReportModifier
    {
    public:
        CollabReportModifier(const std::string& projectPath, std::shared_ptr<FileSystem> fileSystem, std::shared_ptr<AttachmentFactory> attachmentFactory)
            : m_ProjectPath(projectPath)
            , m_FileSystem(fileSystem)
            , m_AttachmentFactory(attachmentFactory)
        {}

        void Modify(ReportDraft& report) const
        {
            auto collabFolder = m_ProjectPath + "/Library/Collab/";

            if (IsEmpty(m_ProjectPath) || !m_FileSystem->HasPath(collabFolder))
                return;

            auto entries = m_FileSystem->GetDirEntries(collabFolder, "");

            auto collabSnapshotPattern = std::string("CollabSnapshot_");
            for (auto it = entries.begin(); it != entries.end(); ++it)
            {
                auto filename = path_utils::GetBaseName(*it);
                if (filename.compare(0, collabSnapshotPattern.length(), collabSnapshotPattern) == 0)
                    report.Attach(m_AttachmentFactory->CreateFileAttachment(*it, "Unity Collab snapshot file"));
            }
        }

    private:
        std::string m_ProjectPath;
        std::shared_ptr<FileSystem> m_FileSystem;
        std::shared_ptr<AttachmentFactory> m_AttachmentFactory;
    };
}
