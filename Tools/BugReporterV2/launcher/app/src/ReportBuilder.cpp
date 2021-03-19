#include "ReportBuilder.h"
#include "Args.h"
#include "reporter/ReportDraft.h"
#include "attachment/Attachment.h"
#include "attachment/AttachmentFactory.h"
#include "attachment_preview/AttachmentTextPreview.h"
#include "file_system/FileSystem.h"
#include "shims/logical/IsEmpty.h"
#include "shims/attribute/GetSize.h"

#include <memory>
#include <algorithm>

namespace ureport
{
namespace details
{
    class ReportBuilderImpl : public ureport::ReportBuilder
    {
        std::shared_ptr<AttachmentFactory> m_AttachmentFactory;
        std::shared_ptr<FileSystem> m_FileSystem;

    public:
        ReportBuilderImpl(std::shared_ptr<AttachmentFactory> attachmentFactory,
                          std::shared_ptr<FileSystem> fileSystem)
            : m_AttachmentFactory(attachmentFactory)
            , m_FileSystem(fileSystem)
        {
        }

    private:
        std::unique_ptr<ReportDraft> BuildReport(const Args& args)
        {
            auto report = CreateNewReport();
            SetEditorMode(*report, args);
            AttachUnityProject(*report, args);
            AttachOtherPaths(*report, args);
            SetBugType(*report, args);
            return report;
        }

        void SetEditorMode(ReportDraft& report, const Args& args)
        {
            auto editorModeValues = args.GetValues("editor_mode");
            if (IsEmpty(editorModeValues))
                return;
            report.WriteFact("UnityEditorMode", editorModeValues[0]);
        }

        void AttachUnityProject(ReportDraft& report, const Args& args)
        {
            auto unityProject = args.GetValues("unity_project");
            if (IsEmpty(unityProject) || !m_FileSystem->HasPath(unityProject[0]))
                return;
            report.Attach(m_AttachmentFactory->CreateDirectoryAttachment(
                unityProject[0], "Unity project"));
        }

        void AttachOtherPaths(ReportDraft& report, const Args& args)
        {
            auto attachments = args.GetValues("attach");
            if (IsEmpty(attachments))
                return;
            std::for_each(attachments.begin(), attachments.end(),
                [this, &report] (const std::string& path)
                {
                    if (!m_FileSystem->HasPath(path))
                        return;
                    if (m_FileSystem->IsDir(path))
                        report.Attach(m_AttachmentFactory->CreateDirectoryAttachment(path, ""));
                    else
                        report.Attach(m_AttachmentFactory->CreateFileAttachment(path, ""));
                });
        }

        void SetBugType(ReportDraft& report, const Args& args)
        {
            auto bugTypeValues = args.GetValues("bugtype");
            if (IsEmpty(bugTypeValues) || GetSize(bugTypeValues) != 1)
                return;
            if (bugTypeValues[0] == "crash")
                report.SetBugTypeID(ureport::Report::kBugCrashAuto);
        }
    };
}

    std::unique_ptr<ReportBuilder> CreateReportBuilder(
        std::shared_ptr<AttachmentFactory> attachmentFactory,
        std::shared_ptr<FileSystem> fileSystem)
    {
        return std::unique_ptr<ReportBuilder>(
            new details::ReportBuilderImpl(attachmentFactory, fileSystem));
    }
}
