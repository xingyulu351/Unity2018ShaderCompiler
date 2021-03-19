#pragma once

#include "reporter/Collector.h"
#include "reporter/ReportDraft.h"
#include "file_system/FileGateway.h"
#include "UnityLogsUtils.h"
#include "common/PathUtils.h"
#include "shims/logical/IsEmpty.h"
#include "attachment/Attachment.h"
#include "attachment/AttachmentFactory.h"
#include "attachment/AttachmentTypes.h"
#include "attachment_preview/AttachmentTextPreview.h"

#include <string>
#include <memory>
#include <utility>

namespace ureport
{
namespace unity_collectors
{
    class UnityLogsCollector : public ureport::Collector
    {
    public:
        UnityLogsCollector(std::shared_ptr<FileGateway> files,
                           std::shared_ptr<FileSystem> fileSystem)
            : m_Files(files)
            , m_FileSystem(fileSystem)
            , m_AttachmentFactory(CreateAttachmentFactory(m_FileSystem, m_Files))
        {
        }

        void Collect(ReportDraft& report) const
        {
            AddLogFile(report, "Editor.log", "Unity Editor log");
            AddLogFile(report, "Editor-prev.log", "Previous Unity Editor log");
            AddLogFile(report, "Player.log", "Standalone player log");
            const std::string logsDir = unity_logs_utils::GetLogsLocation();
            AddDir(report, GetMonoDevelopLogsDir("4.0"),
                "MonoDevelop-Unity-4.0-Logs", "MonoDevelop 4 logs");
            AddDir(report, GetMonoDevelopLogsDir("5.0"),
                "MonoDevelop-Unity-5.0-Logs", "MonoDevelop 5 logs");
        }

    private:
        void AddLogFile(ReportDraft& report,
            const std::string& logFileName,
            const std::string& description) const
        {
            try
            {
                auto path = unity_logs_utils::GetUnityLogsLocation() +
                    logFileName;
                if (!IsEmpty(path))
                {
                    auto attachment = m_AttachmentFactory->CreateFileAttachment(
                        path, description);
                    attachment->GetTextPreview()->SetDescription(description);
                    attachment->GetTextPreview()->SetRemovable(false);
                    report.Attach(std::move(attachment));
                }
            }
            catch (...)
            {
            }
        }

        void AddDir(ReportDraft& report, const std::string& path,
            const std::string& name, const std::string& description) const
        {
            try
            {
                std::map<std::string, std::string> data;
                data["Path"] = path;
                data["Name"] = name;
                data["Description"] = description;
                auto attachment = m_AttachmentFactory->CreateAttachment(
                    attachment::kDirectoryType, data);
                attachment->GetTextPreview()->SetDescription(description);
                attachment->GetTextPreview()->SetRemovable(false);
                report.Attach(std::move(attachment));
            }
            catch (...)
            {
            }
        }

        std::string GetMonoDevelopLogsDir(const std::string& version) const
        {
            const std::string logDir = unity_logs_utils::GetLogsLocation() + "MonoDevelop-Unity-" + version;
#ifdef WINDOWS
            return logDir + "/Logs";
#else
            return logDir;
#endif
        }

    private:
        std::shared_ptr<FileGateway> m_Files;
        std::shared_ptr<FileSystem> m_FileSystem;
        std::shared_ptr<AttachmentFactory> m_AttachmentFactory;
    };
}
}
