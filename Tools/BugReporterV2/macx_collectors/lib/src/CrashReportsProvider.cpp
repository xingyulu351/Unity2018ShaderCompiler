#include "CrashReportsProvider.h"
#include "CrashReport.h"
#include "file_system/FileSystem.h"

#include <algorithm>
#include <iterator>

namespace ureport
{
namespace macosx
{
    const std::string CrashReportsProvider::kDiagnosticReportsPath =
        "~/Library/Logs/DiagnosticReports";

    const std::string CrashReportsProvider::kCrashReporterPath =
        "~/Library/Logs/CrashReporter";

    CrashReportsProvider::CrashReportsProvider(std::shared_ptr<FileSystem> fileSystem)
        : m_FileSystem(fileSystem)
    {
    }

    CrashReportsProvider::~CrashReportsProvider()
    {
    }

    std::vector<CrashReport> CrashReportsProvider::GetAllReports() const
    {
        auto const files = GetAllReportFiles();
        std::vector<CrashReport> reports;
        std::for_each(files.cbegin(), files.cend(),
            [&] (const std::string& file)
            {
                if (IsCrashReportPath(file))
                    reports.push_back(CrashReport::MakeFromPath(file));
            });
        return reports;
    }

    std::list<std::string> CrashReportsProvider::GetAllReportFiles() const
    {
        return m_FileSystem->HasPath(kDiagnosticReportsPath)
            ? m_FileSystem->GetDirEntries(kDiagnosticReportsPath, "")
            : m_FileSystem->GetDirEntries(kCrashReporterPath, "");
    }

    bool CrashReportsProvider::IsCrashReportPath(const std::string& path) const
    {
        const std::string suffix = ".crash";
        return path.find(suffix, path.size() - suffix.size()) != std::string::npos;
    }
}
}
