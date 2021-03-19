#pragma once

#include "CrashReport.h"
#include "CrashReportsProvider.h"
#include "reporter/Collector.h"
#include "reporter/ReportDraft.h"
#include "attachment/Attachment.h"
#include "attachment/AttachmentFactory.h"
#include "attachment/AttachmentTypes.h"

#include <algorithm>
#include <iterator>
#include <functional>

namespace ureport
{
namespace collectors
{
    class UnityCrashLogsCollector : public ureport::Collector
    {
        std::shared_ptr<macosx::CrashReportsProvider> m_ReportsProvider;
        std::shared_ptr<AttachmentFactory> m_AttachmentFactory;

    public:
        UnityCrashLogsCollector(std::shared_ptr<macosx::CrashReportsProvider> reports,
                                std::shared_ptr<AttachmentFactory> attachments)
            : m_ReportsProvider(reports)
            , m_AttachmentFactory(attachments)
        {
        }

        void Collect(ReportDraft& report) const
        {
            auto const reports = GetUnityReports(2);
            std::for_each(reports.begin(), reports.end(),
                [&] (const macosx::CrashReport& crashReport)
                {
                    report.Attach(MakeAttachment(crashReport));
                });
        }

    private:
        std::vector<macosx::CrashReport> GetUnityReports(size_t number) const
        {
            auto reports = m_ReportsProvider->GetAllReports();
            SortReportsByTimestamp(reports);
            std::vector<macosx::CrashReport> unityReports;
            size_t count = 0;
            std::copy_if(reports.begin(), reports.end(), std::back_inserter(unityReports),
                [&] (const macosx::CrashReport& report)
                {
                    return IsUnityReport(report) && ++count <= number;
                });
            return unityReports;
        }

        void SortReportsByTimestamp(std::vector<macosx::CrashReport>& reports) const
        {
            std::sort(reports.begin(), reports.end(),
                [] (const macosx::CrashReport& first, const macosx::CrashReport& second)
                {
                    return first.GetTimestamp() > second.GetTimestamp();
                });
        }

        bool IsUnityReport(const macosx::CrashReport& report) const
        {
            return report.GetApplication() == "Unity";
        }

        std::unique_ptr<Attachment> MakeAttachment(const macosx::CrashReport& report) const
        {
            std::map<std::string, std::string> properties;
            properties["Path"] = report.GetPath();
            properties["Description"] = "Unity crash log";
            properties["Removable"] = "no";
            return m_AttachmentFactory->CreateAttachment(attachment::kFileType, properties);
        }
    };
}
}
