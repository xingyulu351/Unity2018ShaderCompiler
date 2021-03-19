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
    class UnityPackageManagerReportModifier : public ReportModifier
    {
    public:
        UnityPackageManagerReportModifier(const std::string& projectPath, std::shared_ptr<FileSystem> fileSystem, std::shared_ptr<AttachmentFactory> attachmentFactory)
            : m_ProjectPath(projectPath)
            , m_FileSystem(fileSystem)
            , m_AttachmentFactory(attachmentFactory)
        {}

        void Modify(ReportDraft& report) const
        {
            auto packagesFolder = m_ProjectPath + "/Packages/";

            if (IsEmpty(m_ProjectPath) || !m_FileSystem->HasPath(packagesFolder))
                return;

            auto entries = m_FileSystem->GetDirEntries(packagesFolder, "");

            auto manifestJsonFileName = std::string("manifest.json");
            for (auto it = entries.begin(); it != entries.end(); ++it)
            {
                auto filename = path_utils::GetBaseName(*it);
                if (filename.compare(manifestJsonFileName) == 0)
                    report.Attach(m_AttachmentFactory->CreateFileAttachment(*it, "Unity Package Manager's Manifest"));
            }
        }

    private:
        std::string m_ProjectPath;
        std::shared_ptr<FileSystem> m_FileSystem;
        std::shared_ptr<AttachmentFactory> m_AttachmentFactory;
    };
}
